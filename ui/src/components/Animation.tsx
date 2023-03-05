enum RainbowMode {
  Solid,
  Linear,
  Center,
}

enum SpawnPosition {
  Side,
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

enum PulseMode {
  Linear,
  NonLinear,
}

export enum AnimationType {
  Rainbow,
  Sparkle,
  Gradient,
  Static,
  ColorBar,
  RainbowMotion,
  GradientMotion,
  Pulse,
  FSEQ = 255,
}

export const AnimationMode = {
  [AnimationType.Rainbow]: RainbowMode,
  [AnimationType.Sparkle]: SpawnPosition,
  [AnimationType.Gradient]: GradientMode,
  [AnimationType.ColorBar]: ColorBarMode,
  [AnimationType.RainbowMotion]: RainbowMode,
  [AnimationType.GradientMotion]: GradientMode,
  [AnimationType.Pulse]: PulseMode,
} as const;

export enum AnimationDataSource {
  NONE,
  RANDOM,
  ACC_X_RAW,
  ACC_Y_RAW,
  ACC_Z_RAW,
  GY_X_RAW,
  GY_Y_RAW,
  GY_Z_RAW,
  ACC_X_G,
  ACC_Y_G,
  ACC_Z_G,
  GY_X_DEG,
  GY_Y_DEG,
  GY_Z_DEG,
  PITCH,
  ROLL,
  YAW,
  ROLL_COMPENSATED_ACC_X_G,
  PITCH_COMPENSATED_ACC_Y_G,
  AUDIO_FREQUENCY_TRIGGER,
  AUDIO_FREQUENCY_VALUE,
  AUDIO_VOLUME_PEAK,
}

export const getDataSourcesForType = (
  type: AnimationType,
  { hasAudioUnit }: { hasAudioUnit: boolean },
) => {
  switch (type) {
    case AnimationType.RainbowMotion:
    case AnimationType.GradientMotion:
      return [
        AnimationDataSource.ACC_X_G,
        AnimationDataSource.ACC_Y_G,
        AnimationDataSource.ACC_Z_G,
        AnimationDataSource.GY_X_DEG,
        AnimationDataSource.GY_Y_DEG,
        AnimationDataSource.GY_Z_DEG,
        AnimationDataSource.PITCH,
        AnimationDataSource.ROLL,
        AnimationDataSource.ROLL_COMPENSATED_ACC_X_G,
        AnimationDataSource.PITCH_COMPENSATED_ACC_Y_G,
      ];
    case AnimationType.Sparkle:
      return [
        AnimationDataSource.RANDOM,
        ...(hasAudioUnit ? [AnimationDataSource.AUDIO_FREQUENCY_TRIGGER] : []),
      ];
    case AnimationType.Pulse:
      return [
        AnimationDataSource.NONE,
        ...(hasAudioUnit ? [AnimationDataSource.AUDIO_FREQUENCY_TRIGGER] : []),
      ];
    default:
      return [];
  }
};

export const getAnimationTypes = ({ hasMPU6050 }: { hasMPU6050: boolean }) =>
  Object.entries(AnimationType)
    .filter(([key]) => isNaN(Number(key)))
    .filter(([, value]) => value !== AnimationType.FSEQ)
    .filter(
      ([, value]) =>
        value !== AnimationType.RainbowMotion ||
        (value === AnimationType.RainbowMotion && hasMPU6050),
    )
    .filter(
      ([, value]) =>
        value !== AnimationType.GradientMotion ||
        (value === AnimationType.GradientMotion && hasMPU6050),
    );
