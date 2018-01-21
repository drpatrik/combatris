#include "game/tetromino_sprite.h"

void TetrominoSprite::RotateClockwise() {
  int angle = static_cast<int>(angle_) + 1;
  auto try_angle = static_cast<Tetromino::Angle>(angle % kAngles.size());
  const auto& rotation_data = tetromino_.GetRotationData(try_angle);

  if (matrix_->IsValid(pos_, rotation_data)) {
    matrix_->Insert(pos_, rotation_data);
    angle_ = try_angle ;
    rotation_data_ = rotation_data;
  }
}

void TetrominoSprite::RotateCounterClockwise() {
  int angle = static_cast<int>(angle_) - 1;

  if (angle < 0) {
    angle = static_cast<int>(Tetromino::Angle::A270);
  }
  auto try_angle = static_cast<Tetromino::Angle>(angle);
  const auto& rotation_data = tetromino_.GetRotationData(try_angle);

  if (matrix_->IsValid(pos_, rotation_data)) {
    matrix_->Insert(pos_, rotation_data);
    angle_ = try_angle;
    rotation_data_ = rotation_data;
  }
}

void TetrominoSprite::HardDrop() {
  pos_ = matrix_->GetDropPosition(pos_, rotation_data_);
  level_.Release();
  floor_reached_ = true;
}

void TetrominoSprite::Left() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() - 1), rotation_data_)) {
    pos_.dec_col();
    matrix_->Insert(pos_, rotation_data_);
  }
}

void TetrominoSprite::Right() {
  if (matrix_->IsValid(Position(pos_.row(), pos_.col() + 1), rotation_data_)) {
    pos_.inc_col();
    matrix_->Insert(pos_, rotation_data_);
  }
}

std::tuple<bool, Matrix::Lines, Matrix::Lines> TetrominoSprite::MoveDown(double delta_time) {
  Matrix::Lines cleared_lines;
  Matrix::Lines lines_to_movedown;
  bool next_piece = false;

  if (level_.Wait(delta_time, floor_reached_)) {
    if (floor_reached_) {
      std::tie(cleared_lines, lines_to_movedown) = matrix_->Commit(pos_, rotation_data_);
      next_piece = true;
    } else {
      if (matrix_->IsValid(Position(pos_.row() + 1, pos_.col()), rotation_data_)) {
        pos_.inc_row();
        matrix_->Insert(pos_, rotation_data_);
      } else {
        floor_reached_ = true;
        can_move_ = (pos_.row() >= kVisibleRowStart - 1);
      }
    }
  }
  return std::make_tuple(next_piece, cleared_lines, lines_to_movedown);
}
