#include "game/tetrion.h"

#include <iostream>

namespace {

const int kSinglePlayerCountDown = 3;
const int kMultiPlayerCountDown = 9;
const char* kWindowTitleSinglePlayer = "COMBATRIS - Single Player";
const SDL_Rect kSinglePlayerRC =  { 0, 0, kWidth, kHeight };
const char* kWindowTitleBattle = "COMBATRIS - Battle";
const SDL_Rect kBattleRC =  { 0, 0, kWidth + kMultiPlayerWidthAddOn, kHeight };

void SetupCampaignWindow(SDL_Window* window, SDL_Renderer* renderer, Tetrion::Campaign campaign) {
  const auto& rc = (campaign == Tetrion::Campaign::SinglePlayer) ? kSinglePlayerRC : kBattleRC;

  SDL_SetWindowSize(window, rc.w, rc.h);
  SDL_RenderSetLogicalSize(renderer, rc.w, rc.h);
}

inline const SDL_Rect& GetWindowRc(Tetrion::Campaign campaign) {
  return (Tetrion::Campaign::SinglePlayer == campaign) ? kSinglePlayerRC : kBattleRC;
}

void RenderWindowBackground(SDL_Renderer* renderer, const SDL_Rect& rc) {
  SDL_SetRenderDrawColor(renderer, 1, 40, 135, 255);
  SDL_RenderFillRect(renderer, &rc);
}

bool RenderAnimations(std::deque<std::shared_ptr<Animation>>& animations, double delta_time, Events& events) {
  for (auto it = animations.begin(); it != animations.end();) {
    (*it)->Render(delta_time);
    if (auto [status, event] = (*it)->IsReady(); status) {
      events.Push(event);
      it = animations.erase(it);
    } else {
      ++it;
    }
  }
  return (animations.size() == 0);
}

template <class T>
void RemoveAnimation(std::deque<std::shared_ptr<Animation>>& animations) {
  animations.erase(std::remove_if(animations.begin(), animations.end(),
    [](const auto &a) { return a->name() == typeid(T).name(); }), animations.end());
}

template <class T>
bool IsAnimationActive(std::deque<std::shared_ptr<Animation>>& animations) {
  return std::find_if(animations.begin(), animations.end(),
                      [](const auto& a) { return a->name() == typeid(T).name(); }) != animations.end();
}

} // namespace

Tetrion::Tetrion() : events_() {
  window_ = SDL_CreateWindow(kWindowTitleSinglePlayer, SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  SetupCampaignWindow(window_, renderer_, campaign_);
  SDL_RaiseWindow(window_);
  assets_ = std::make_shared<Assets>(renderer_);
  matrix_ = std::make_shared<Matrix>(renderer_, assets_->GetTetrominos());
  AddPane(matrix_.get());
  level_ = std::make_shared<Level>(renderer_, events_, assets_);
  AddPane(level_.get());
  tetromino_generator_ = std::make_shared<TetrominoGenerator>(matrix_, level_, events_, assets_);
  scoring_ = std::make_unique<Scoring>(renderer_, assets_, level_);
  AddPane(scoring_.get());
  high_score_ = std::make_unique<HighScore>(renderer_, assets_);
  AddPane(high_score_.get());
  next_queue_ = std::make_unique<NextQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(next_queue_.get());
  hold_queue_ = std::make_unique<HoldQueue>(renderer_, tetromino_generator_, assets_);
  AddPane(hold_queue_.get());
  total_lines_ = std::make_unique<TotalLines>(renderer_, assets_);
  AddPane(total_lines_.get());
  moves_ = std::make_unique<Moves>(renderer_, assets_);
  AddPane(moves_.get());
  AddAnimation<SplashScreenAnimation>(renderer_, assets_);
  multi_player_ = std::make_shared<MultiPlayer>(renderer_, events_, assets_);
  AddPane(multi_player_.get());
}

Tetrion::~Tetrion() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Tetrion::ToggleCampaign() {
  if (tetromino_in_play_) {
    return;
  }
  if (Campaign::SinglePlayer == campaign_) {
    multi_player_->Enable();
    campaign_ = Campaign::Battle;
    auto title = std::string(kWindowTitleBattle) + " (" + multi_player_->our_host_name() + " )";
    SDL_SetWindowTitle(window_, title.c_str());
  } else {
    multi_player_->Disable();
    campaign_ = Campaign::SinglePlayer;
    SDL_SetWindowTitle(window_, kWindowTitleSinglePlayer);
  }
  SetupCampaignWindow(window_, renderer_, campaign_);
}

void Tetrion::GameControl(Controls control_pressed) {
  if (!tetromino_in_play_ || game_paused_) {
    return;
  }
  switch (control_pressed) {
    case Controls::RotateClockwise:
      tetromino_in_play_->RotateClockwise();
      break;
    case Controls::RotateCounterClockwise:
      tetromino_in_play_->RotateCounterClockwise();
      break;
    case Controls::SoftDrop:
      tetromino_in_play_->SoftDrop();
      break;
    case Controls::HardDrop:
      tetromino_in_play_->HardDrop();
      break;
    case Controls::Left:
      tetromino_in_play_->Left();
      break;
    case Controls::Right:
      tetromino_in_play_->Right();
      break;
    case Controls::Hold:
      if (hold_queue_->CanHold()) {
        RemoveAnimation<OnFloorAnimation>(animations_);
        tetromino_in_play_ = hold_queue_->Hold(tetromino_in_play_);
      }
      break;
#if !defined(NDEBUG)
    case Controls::Send1Line:
      multi_player_->DebugSend(1);
      break;
    case Controls::Send5Lines:
      multi_player_->DebugSend(5);
      break;
    case Controls::Send8Lines:
      multi_player_->DebugSend(8);
      break;
#endif
    default:
      break;
  }
}

void Tetrion::HandleNextTetromino(TetrominoSprite::State state, Events& events) {
  switch (state) {
    case TetrominoSprite::State::Falling:
      if (Campaign::SinglePlayer == campaign_) {
        break;
      }
      events.Push(Event::Type::BattleNextTetrominoSuccessful);
      break;
    case TetrominoSprite::State::GameOver:
      tetromino_in_play_.reset();
      events.PushFront(Event::Type::GameOver);
      break;
    case TetrominoSprite::State::KO:
      matrix_->RemoveLines();
      tetromino_generator_->Put(tetromino_in_play_->tetromino());
      tetromino_in_play_.reset();
      AddAnimation<MessageAnimation>(renderer_, assets_, "Got K.O. :-(", Color::Red, 100.0);
      events.Push(Event::Type::NextTetromino);
      events.Push(Event::Type::BattleKnockedOut);
      break;
    default:
      break;
  }
}

void Tetrion::EventHandler(Events& events) {
  if (events.IsEmpty()) {
    return;
  }
  auto event = events.Pop();

  std::for_each(event_listeners_.begin(), event_listeners_.end(), [&event](const auto& r) { r->Update(event); });

  switch (event.type()) {
    case Event::Type::Pause:
      next_queue_->Hide();
      AddAnimation<PauseAnimation>(renderer_, assets_, unpause_pressed_);
      break;
    case Event::Type::UnPause:
      AddAnimation<CountDownAnimation>(renderer_, assets_, kSinglePlayerCountDown, Event::Type::CountdownAfterUnPauseDone);
      break;
    case Event::Type::CountdownAfterUnPauseDone:
      next_queue_->Show();
      level_->ResetTime();
      unpause_pressed_ = game_paused_ = false;
      break;
    case Event::Type::NextTetromino:
    case Event::Type::BattleNextTetrominoGotLines:
      next_queue_->Show();
      tetromino_in_play_ = tetromino_generator_->Get(event.Is(Event::Type::BattleNextTetrominoGotLines));
      HandleNextTetromino(tetromino_in_play_->state(), events);
      break;
    case Event::Type::LevelUp:
      AddAnimation<MessageAnimation>(renderer_, assets_, "LEVEL UP", Color::SteelGray, 100);
      break;
    case Event::Type::LinesCleared:
      RemoveAnimation<LinesClearedAnimation>(animations_);
      AddAnimation<LinesClearedAnimation>(renderer_, assets_, event.lines_cleared_);
      break;
    case Event::Type::CalculatedScore:
      if (Campaign::SinglePlayer == campaign_) {
        AddAnimation<ScoreAnimation>(renderer_, assets_, event.pos_, event.score_);
      }
      break;
    case Event::Type::NewGame:
      if (!multi_player_->CanPressNewGame()) {
        break;
      }
      animations_.clear();
      events.Clear();
      next_queue_->Hide();
      tetromino_generator_->Reset();
      unpause_pressed_ = game_paused_ = false;
      std::for_each(panes_.begin(), panes_.end(), [](const auto& r) { r->Reset(); });
      if (Campaign::SinglePlayer == campaign_) {
        AddAnimation<CountDownAnimation>(renderer_, assets_, kSinglePlayerCountDown, Event::Type::NextTetromino);
      } else {
        multi_player_->NewGame();
      }
      break;
    case Event::Type::GameOver:
      events.Clear();
      animations_.clear();
      AddAnimation<GameOverAnimation>(renderer_, assets_);
      break;
    case Event::Type::OnFloor:
      AddAnimation<OnFloorAnimation>(renderer_, assets_, tetromino_in_play_);
      break;
    case Event::Type::Falling:
      RemoveAnimation<OnFloorAnimation>(animations_);
      break;
    case Event::Type::BattleStartGame:
      multi_player_->StartGame();
      break;
    case Event::Type::BattleWaitForPlayers:
      if (multi_player_->CanStartGame()) {
        events_.Push(Event::Type::NextTetromino);
      } else {
        AddAnimation<HourglassAnimation>(renderer_, assets_, multi_player_);
      }
      break;
    case Event::Type::BattleSendLines:
      AddAnimation<MessageAnimation>(renderer_, assets_, "Sent " + std::to_string(event.value_) + " lines", Color::SteelGray, 200.0);
      break;
    case Event::Type::BattleResetCountDown:
      RemoveAnimation<CountDownAnimation>(animations_);
      AddAnimation<CountDownAnimation>(renderer_, assets_, kMultiPlayerCountDown, Event::Type::BattleStartGame);
      break;
    case Event::Type::BattleGotLines:
      if (!tetromino_in_play_) {
        break;
      }
      tetromino_generator_->Put(tetromino_in_play_->tetromino());
      tetromino_in_play_.reset();
      matrix_->InsertLines(event.value_);
      events_.Push(Event::Type::BattleNextTetrominoGotLines);
      break;
    case Event::Type::BattleYouDidKO:
      AddAnimation<MessageAnimation>(renderer_, assets_, "*** K.O. ***", Color::Gold, 100.0);
      break;
    default:
      break;
  }
}

void Tetrion::Render(double delta_time) {
  SDL_RenderClear(renderer_);

  RenderWindowBackground(renderer_, GetWindowRc(campaign_));

  std::for_each(panes_.begin(), panes_.end(), [delta_time](const auto& pane) { pane->Render(delta_time); });

  RenderAnimations(animations_, delta_time, events_);

  SDL_RenderPresent(renderer_);
}

void Tetrion::Update(double delta_time) {
  EventHandler(events_);
  if (!game_paused_) {
    if (tetromino_in_play_ && tetromino_in_play_->Down(delta_time) == TetrominoSprite::State::Commited) {
      tetromino_in_play_.reset();
      events_.Push(Event::Type::NextTetromino);
    }
  }
  Render(delta_time);
}
