#include "game/tetrion.h"

#include <iostream>

namespace {

const int kSinglePlayerCountDown = 3;
const int kMultiPlayerCountDown = 9;

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

std::string RankToText(int rank) {
  const std::vector<std::string> kRanks = { "Winner", "2nd place", "3rd place", "4th place", "5th place", "6th place" };

  return kRanks.at(rank);
}

} // namespace

using namespace utility;

Tetrion::Tetrion() : events_() {
  window_ = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
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
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_RaiseWindow(window_);
  assets_ = std::make_shared<Assets>(renderer_);
  matrix_ = std::make_shared<Matrix>(renderer_, assets_->GetTetrominos());
  campaign_ = std::make_shared<Campaign>(window_, renderer_, events_, assets_, matrix_);
  hold_queue_ = campaign_->GetHoldQueuePane();
  multi_player_ = campaign_->GetMultiPlayerPane();
  tetromino_generator_ = campaign_->GetTetrominoGenerator();
  combatris_menu_ = std::make_shared<CombatrisMenu>(events_);
  AddAnimation<SplashScreenAnimation>(renderer_, combatris_menu_, assets_);

  events_.Push(Event::Type::MenuSetModeAndCampaign, ModeType::SinglePlayer, CampaignType::Combatris);
}

Tetrion::~Tetrion() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Tetrion::HandleMenu(Controls control_pressed) {
  if (!(IsAnimationActive<SplashScreenAnimation>(animations_) || IsAnimationActive<GameOverAnimation>(animations_))) {
    return;
  }
  switch (control_pressed) {
    case Controls::Up:
      combatris_menu_->Previous();
      break;
    case Controls::Down:
      combatris_menu_->Next();
      break;
    case Controls::Left:
      combatris_menu_->PrevSubItem();
      break;
    case Controls::Right:
      combatris_menu_->NextSubItem();
      break;
    default:
      break;
  }
}

void Tetrion::GameControl(Controls control_pressed, int lines) {
  if (!tetromino_in_play_ || game_paused_) {
    HandleMenu(control_pressed);
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
    case Controls::DebugSendLine:
      multi_player_->DebugSend(lines);
      break;
    default:
      break;
  }
}

void Tetrion::HandleNextTetromino(TetrominoSprite::State state, Events& events) {
  switch (state) {
    case TetrominoSprite::State::Falling:
      if (!IsBattleCampaign(*campaign_)) {
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
  auto event = campaign_->PreprocessEvent(events.Pop());

  switch (event.type()) {
    case Event::Type::Pause:
      campaign_->Pause();
      AddAnimation<PauseAnimation>(renderer_, assets_, unpause_pressed_);
      break;
    case Event::Type::UnPause:
      AddAnimation<CountDownAnimation>(renderer_, assets_, kSinglePlayerCountDown, Event::Type::CountdownAfterUnPauseDone);
      break;
    case Event::Type::CountdownAfterUnPauseDone:
      campaign_->Unpause();
      unpause_pressed_ = game_paused_ = false;
      break;
    case Event::Type::NextTetromino:
    case Event::Type::BattleNextTetrominoGotLines:
      campaign_->ShowNextQueue();
      tetromino_in_play_ = tetromino_generator_->Get(event.Is(Event::Type::BattleNextTetrominoGotLines));
      HandleNextTetromino(tetromino_in_play_->state(), events);
      break;
    case Event::Type::LevelUp:
      AddAnimation<MessageAnimation>(renderer_, assets_, "LEVEL UP", Color::SteelGray, 100);
      break;
    case Event::Type::LinesCleared:
      if (event.lines_.size() > 0) {
	RemoveAnimation<LinesClearedAnimation>(animations_);
	AddAnimation<LinesClearedAnimation>(renderer_, assets_, event.lines_);
      }
      break;
    case Event::Type::CalculatedScore:
      if (!IsBattleCampaign(*campaign_)) {
        AddAnimation<ScoreAnimation>(renderer_, assets_, event.pos_, event.score_);
      }
      break;
    case Event::Type::NewGame:
      if (!multi_player_->CanPressNewGame()) {
        break;
      }
      animations_.clear();
      events.Clear();
      unpause_pressed_ = game_paused_ = false;
      campaign_->Reset();
      if (campaign_->IsSinglePlayer()) {
        AddAnimation<CountDownAnimation>(renderer_, assets_, kSinglePlayerCountDown, Event::Type::NextTetromino);
      } else {
        multi_player_->NewGame();
      }
      break;
    case Event::Type::MultiplayerCampaignOver:
      RemoveAnimation<HourglassAnimation>(animations_);
      AddAnimation<GameOverAnimation>(renderer_, combatris_menu_, assets_, RankToText(static_cast<int>(event.value_)));
      break;
    case Event::Type::GameOver:
    case Event::Type::MultiplayerRejected:
      events.Clear();
      animations_.clear();
      tetromino_in_play_.reset();
      if (campaign_->IsSinglePlayer()) {
        const auto text = (event.value_ == 1) ? "Winner" : "Game Over";

        AddAnimation<GameOverAnimation>(renderer_, combatris_menu_, assets_, text);
      } else {
        if (Event::Type::MultiplayerRejected == event.type()) {
          AddAnimation<GameOverAnimation>(renderer_, combatris_menu_, assets_, "You where rejected");
        } else {
          AddAnimation<HourglassAnimation>(renderer_, assets_);
        }
      }
      break;
    case Event::Type::OnFloor:
      AddAnimation<OnFloorAnimation>(renderer_, assets_, tetromino_in_play_);
      break;
    case Event::Type::ClearOnFloor:
      RemoveAnimation<OnFloorAnimation>(animations_);
      break;
    case Event::Type::PerfectClear:
      AddAnimation<MessageAnimation>(renderer_, assets_, "PERFECT CLEAR", Color::Gold, 100.0);
      break;
    case Event::Type::BattleSendLines:
      AddAnimation<MessageAnimation>(renderer_, assets_, "Sent " + std::to_string(event.value_) + " lines", Color::SteelGray, 200.0);
      break;
    case Event::Type::MultiplayerResetCountDown:
      RemoveAnimation<CountDownAnimation>(animations_);
      AddAnimation<CountDownAnimation>(renderer_, assets_, kMultiPlayerCountDown, Event::Type::MultiplayerStartGame);
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
    default:
      break;
  }
}

void Tetrion::Render(double delta_time) {
  SDL_RenderClear(renderer_);
  campaign_->Render(delta_time);
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
