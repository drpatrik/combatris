#pragma once

#include "game/panes/total_lines.h"
#include "game/panes/scoring.h"
#include "game/panes/next_piece.h"
#include "game/panes/hold_piece.h"
#include "game/animation.h"

class Board final {
 public:
  enum class Controls { RotateClockwise, RotateCounterClockwise, SoftDrop, HardDrop, Left, Right, HoldPiece, Pause };

  Board();

  Board(const Board&) = delete;

  Board(const Board&&) = delete;

  ~Board() noexcept;

  void NewGame();

  void GameControl(Controls control_pressed);

  void Update(double delta_timer);

 protected:
  template<class T, class ...Args>
  void StartAnimation(Args&&... args) {
    auto animation = std::make_shared<T>(std::forward<Args>(args)...);

    animation->Start();
    animations_.push_front(animation);
  }
  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  std::unique_ptr<TetrominoSprite> tetromino_in_play_;
  std::shared_ptr<Assets> assets_;
  std::shared_ptr<TetrominoGenerator> tetromino_generator_;
  std::shared_ptr<Matrix> matrix_;
  std::shared_ptr<Level> level_;
  std::unique_ptr<Scoring> scoring_;
  std::unique_ptr<NextPiece> next_piece_;
  std::unique_ptr<HoldPiece> hold_piece_;
  std::unique_ptr<TotalLines> total_lines_;
  std::vector<PaneInterface*> pane_list_;
  Events events_;
  std::deque<std::shared_ptr<Animation>> animations_;
};
