enum RainbowMode {
  Solid,
  Linear,
  Center,
}

enum SpawnPosition {
  Left,
  Right,
  Center,
  Random,
}

enum GradientMode {
  Linear,
  Center,
}

enum ColorBarMode {
  LinearHardBorders,
  LinearSmoothBorder,
  CenterHardBorders,
  CenterSmoothBorder,
}

export enum AnimationType {
  Rainbow,
  Sparkle,
  Gradient,
  Static,
  ColorBar,
  RainbowMotion,
  GradientMotion,
  FSEQ = 255,
}

export const AnimationMode = {
  [AnimationType.Rainbow]: RainbowMode,
  [AnimationType.Sparkle]: SpawnPosition,
  [AnimationType.Gradient]: GradientMode,
  [AnimationType.ColorBar]: ColorBarMode,
  [AnimationType.RainbowMotion]: RainbowMode,
  [AnimationType.GradientMotion]: GradientMode,
} as const;

export enum MotionSensorValue {
  ACC_X_G = 6,
  ACC_Y_G = 7,
  ACC_Z_G = 8,
  GY_X_DEG = 9,
  GY_Y_DEG = 10,
  GY_Z_DEG = 11,
  PITCH = 12,
  ROLL = 13,
  ROLL_COMPENSATED_ACC_X_G = 15,
  PITCH_COMPENSATED_ACC_Y_G = 16,
}
