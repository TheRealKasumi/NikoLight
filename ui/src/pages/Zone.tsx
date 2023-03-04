import { ArrowsRightLeftIcon } from '@heroicons/react/24/outline';
import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { useMatch } from '@tanstack/react-router';
import hexRgb from 'hex-rgb';
import { Suspense, useCallback, useEffect, useMemo } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import { useTranslation } from 'react-i18next';
import rgbHex from 'rgb-hex';
import {
  AnimationDataSource,
  AnimationMode,
  AnimationType,
  Button,
  Collapsible,
  ColorPicker,
  Error,
  getAnimationTypes,
  getDataSourcesForType,
  Loading,
  Notification,
  Select,
  SelectItem,
  Slider,
  Switch,
  Toast,
  ToggleGroup,
  ToggleGroupItem,
} from '../components';
import { toPercentage } from '../libs';
import { useLed, useSystemInfo, useUpdateLed } from '../pages/api';

type FormData = {
  animationMode: string;
  animationSettings: number[];
  brightness: number;
  channelCurrents: number;
  color1: string;
  color2: string;
  frequencyBands: string[];
  dataSource: string;
  fadeSpeed: number;
  ledCount: number;
  ledVoltage: number;
  offset: number;
  reverse: boolean;
  speed: number;
  type: string;
};

const DEFAULT_VALUES: FormData = {
  animationMode: '0',
  animationSettings: [...Array(25)].fill(0),
  brightness: 50,
  channelCurrents: 16,
  color1: '#000000',
  color2: '#000000',
  dataSource: AnimationDataSource.NONE.toString(),
  fadeSpeed: 30,
  frequencyBands: [],
  ledCount: 2,
  ledVoltage: 5,
  offset: 10,
  reverse: false,
  speed: 50,
  type: '0',
};

type FormProps = { zoneId: number };

const Form = ({ zoneId }: FormProps): JSX.Element => {
  const { t } = useTranslation();
  const { data: systemInfo } = useSystemInfo();
  const { data } = useLed();
  const { mutateAsync, isSuccess, isError, error } = useUpdateLed();
  const zone = data?.[zoneId];

  const {
    handleSubmit,
    watch,
    control,
    reset,
    formState: { isSubmitting, defaultValues },
    setValue,
    resetField,
  } = useForm<FormData>({
    defaultValues: {
      brightness: zone?.brightness,
      color1: `#${rgbHex(
        zone?.animationSettings[1] ?? 0,
        zone?.animationSettings[2] ?? 0,
        zone?.animationSettings[3] ?? 0,
      )}`,
      color2: `#${rgbHex(
        zone?.animationSettings[4] ?? 0,
        zone?.animationSettings[5] ?? 0,
        zone?.animationSettings[6] ?? 0,
      )}`,
      dataSource: zone?.dataSource.toString(),
      fadeSpeed: zone?.fadeSpeed,
      frequencyBands: [...Array(8).keys()]
        .map((item) => (++item).toString())
        .reverse()
        .filter(
          (_, i) => (zone?.animationSettings[18] ?? 0b00000000) & (1 << i),
        ),
      reverse: zone?.reverse,
      channelCurrents: zone?.channelCurrents[0],
      ledCount: zone?.ledCount,
      ledVoltage: zone?.ledVoltage,
      offset: zone?.offset,
      speed: zone?.speed,
      type: zone?.type.toString(),
      animationMode: zone?.animationSettings[0].toString(),
      animationSettings: zone?.animationSettings,
    },
    mode: 'onChange',
  });

  const onSubmit = handleSubmit(
    async ({
      animationMode,
      animationSettings,
      channelCurrents,
      color1,
      color2,
      frequencyBands,
      dataSource,
      type,
      ...rest
    }) => {
      const led = JSON.parse(JSON.stringify(data)) as NonNullable<typeof data>;

      // Set animation settings
      led[zoneId].animationSettings = animationSettings.map(
        (animationSetting, index) => {
          const rgbColor1 = hexRgb(color1);
          const rgbColor2 = hexRgb(color2);

          switch (index) {
            case 0:
              return Number(animationMode);
            case 1:
              return rgbColor1.red;
            case 2:
              return rgbColor1.green;
            case 3:
              return rgbColor1.blue;
            case 4:
              return rgbColor2.red;
            case 5:
              return rgbColor2.green;
            case 6:
              return rgbColor2.blue;
            case 18:
              return frequencyBands
                .map((item) => 128 / Math.pow(2, Number(item) - 1))
                .reduce((prev, curr) => prev + curr, 0);
            default:
              return Number(animationSetting);
          }
        },
      );

      led[zoneId] = {
        ...led[zoneId],
        ...rest,
        channelCurrents: [...Array(3)].fill(channelCurrents),
        dataSource: Number(dataSource),
        type: Number(type),
      };

      await mutateAsync(led);

      // Reset dirty fields
      reset({}, { keepValues: true });
    },
  );

  const onReset = async () => {
    reset(DEFAULT_VALUES);
    return await onSubmit();
  };

  const onTypeChange = useCallback(
    (type: AnimationType) => {
      // Reset all animation settings
      resetField('animationMode', {
        defaultValue: DEFAULT_VALUES.animationMode,
      });
      resetField('animationSettings', {
        defaultValue: DEFAULT_VALUES.animationSettings,
      });
      resetField('color1', {
        defaultValue: DEFAULT_VALUES.color1,
      });
      resetField('color2', {
        defaultValue: DEFAULT_VALUES.color2,
      });
      resetField('dataSource', {
        defaultValue: DEFAULT_VALUES.dataSource,
      });
      resetField('frequencyBands', {
        defaultValue: DEFAULT_VALUES.frequencyBands,
      });

      switch (Number(type)) {
        case AnimationType.Rainbow:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.Rainbow].Solid.toString(),
          );
          break;

        case AnimationType.Sparkle:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.Sparkle].Center.toString(),
          );
          setValue('color1', `#${rgbHex(0, 0, 0)}`);
          setValue('dataSource', AnimationDataSource.RANDOM.toString());
          setValue('animationSettings.7', 5);
          setValue('animationSettings.10', 229);
          setValue('animationSettings.11', 127);
          break;

        case AnimationType.Gradient:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.Gradient].Linear.toString(),
          );
          setValue('color1', `#${rgbHex(255, 0, 0)}`);
          setValue('color2', `#${rgbHex(0, 0, 255)}`);
          break;

        case AnimationType.Static:
          setValue('color1', `#${rgbHex(255, 0, 0)}`);
          break;

        case AnimationType.ColorBar:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.ColorBar].LinearSmoothBorder.toString(),
          );
          setValue('color1', `#${rgbHex(255, 0, 0)}`);
          setValue('color2', `#${rgbHex(0, 0, 255)}`);
          break;

        case AnimationType.RainbowMotion:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.RainbowMotion].Linear.toString(),
          );
          setValue('dataSource', AnimationDataSource.ACC_X_G.toString());
          break;

        case AnimationType.GradientMotion:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.GradientMotion].Linear.toString(),
          );
          setValue('color1', `#${rgbHex(255, 0, 0)}`);
          setValue('color2', `#${rgbHex(0, 0, 255)}`);
          setValue('dataSource', AnimationDataSource.ACC_X_G.toString());
          break;

        case AnimationType.Pulse:
          setValue(
            'animationMode',
            AnimationMode[AnimationType.Pulse].Linear.toString(),
          );
          setValue('color1', `#${rgbHex(0, 0, 0)}`);
          setValue('dataSource', AnimationDataSource.NONE.toString());
          break;
      }
    },
    [resetField, setValue],
  );

  useEffect(() => {
    const subscription = watch((value, { name, type }) => {
      if (name === 'type' && type === 'change') {
        setTimeout(() => {
          onTypeChange(Number(value.type));
        });
      }
    });
    return () => subscription.unsubscribe();
  }, [onTypeChange, watch]);

  const values = watch();

  const hasMPU6050 = (systemInfo?.hardwareInfo.mpu6050 ?? 0) > 0;
  const hasAudioUnit = (systemInfo?.hardwareInfo.audioUnit ?? 0) > 0;

  const animationTypes = useMemo(
    () => getAnimationTypes({ hasMPU6050 }),
    [hasMPU6050],
  );

  const dataSources = useMemo(
    () => getDataSourcesForType(Number(values.type), { hasAudioUnit }),
    [hasAudioUnit, values.type],
  );

  return (
    <>
      {isSuccess && <Toast title={t('zone.submitSuccessful')} />}

      {isError && <Notification state="error" message={error.message} />}

      <form onSubmit={onSubmit}>
        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('zone.basic')}
          </legend>

          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">
              {t('zone.animationType')}
            </span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="type">
                {animationTypes.map(([key, value]) => (
                  <SelectItem key={key} value={value.toString()}>
                    {t(`zone.animationTypes.${key}`)}
                  </SelectItem>
                ))}
              </Select>
            </div>
          </label>

          {AnimationType.Static !== Number(values.type) && (
            <label className="mb-6 flex flex-row justify-between">
              <span className="basis-1/2 self-center">
                {t('zone.animationMode')}
              </span>
              <div className="basis-1/2 text-right">
                <Select<FormData> control={control} name="animationMode">
                  {Object.entries(
                    AnimationMode[
                      Number(values.type) as keyof typeof AnimationMode
                    ],
                  )
                    .filter(([key]) => isNaN(Number(key)))
                    .map(([key, value]) => (
                      <SelectItem key={key} value={value.toString()}>
                        {t(`zone.animationModes.${key}`)}
                      </SelectItem>
                    ))}
                </Select>
              </div>
            </label>
          )}

          {[
            AnimationType.RainbowMotion,
            AnimationType.GradientMotion,
            AnimationType.Sparkle,
            AnimationType.Pulse,
          ].includes(Number(values.type)) &&
            dataSources.length > 1 && (
              <label className="mb-6 flex flex-row justify-between">
                <span className="basis-1/3 self-center">
                  {t('zone.dataSource')}
                </span>
                <div className="basis-2/3 text-right">
                  <Select<FormData> control={control} name="dataSource">
                    {dataSources.map((value) => (
                      <SelectItem
                        key={AnimationDataSource[value]}
                        value={value.toString()}
                      >
                        {t(
                          `zone.animationDataSources.${AnimationDataSource[value]}`,
                        )}
                      </SelectItem>
                    ))}
                  </Select>
                </div>
              </label>
            )}

          {[AnimationType.Sparkle, AnimationType.Pulse].includes(
            Number(values.type),
          ) &&
            hasAudioUnit &&
            AnimationDataSource.AUDIO_FREQUENCY_TRIGGER ===
              Number(values.dataSource) && (
              <label className="mb-6 flex flex-col">
                <span className="mb-2">{t('zone.frequencyBands')}</span>
                <ToggleGroup<FormData>
                  control={control}
                  name="frequencyBands"
                  type="multiple"
                >
                  <ToggleGroupItem value="1">1</ToggleGroupItem>
                  <ToggleGroupItem value="2">2</ToggleGroupItem>
                  <ToggleGroupItem value="3">3</ToggleGroupItem>
                  <ToggleGroupItem value="4">4</ToggleGroupItem>
                  <ToggleGroupItem value="5">5</ToggleGroupItem>
                  <ToggleGroupItem value="6">6</ToggleGroupItem>
                  <ToggleGroupItem value="7">7</ToggleGroupItem>
                  <ToggleGroupItem value="8">8</ToggleGroupItem>
                </ToggleGroup>
              </label>
            )}

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.brightness')}: {toPercentage(values.brightness)}%
            </span>
            <Slider<FormData>
              className="w-full"
              control={control}
              name="brightness"
              min={0}
              max={255}
              step={1}
            />
          </label>

          {![AnimationType.Gradient, AnimationType.Static].includes(
            Number(values.type),
          ) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('zone.speed')}: {toPercentage(values.speed)}%
              </span>
              <Slider<FormData>
                className="w-full"
                control={control}
                name="speed"
                min={0}
                max={255}
                step={5}
              />
            </label>
          )}

          {![AnimationType.Static, AnimationType.Pulse].includes(
            Number(values.type),
          ) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('zone.offset')}: {toPercentage(values.offset)}%
              </span>
              <Slider<FormData>
                className="w-full"
                control={control}
                name="offset"
                min={0}
                max={255}
                step={5}
              />
            </label>
          )}

          {[
            AnimationType.ColorBar,
            AnimationType.Gradient,
            AnimationType.GradientMotion,
            AnimationType.Sparkle,
            AnimationType.Static,
            AnimationType.Pulse,
          ].includes(Number(values.type)) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">{t('zone.color')}</span>
              <div className="flex">
                <ColorPicker<FormData>
                  className="h-10 grow"
                  control={control}
                  name="color1"
                />
                {[
                  AnimationType.ColorBar,
                  AnimationType.Gradient,
                  AnimationType.GradientMotion,
                ].includes(Number(values.type)) && (
                  <>
                    <button
                      type="button"
                      className="grow-0 px-4 focus:outline-none focus-visible:ring focus-visible:ring-sky focus-visible:ring-opacity-75"
                      onClick={() => {
                        const { color1, color2 } = values;
                        setValue('color1', color2);
                        setValue('color2', color1);
                      }}
                    >
                      <ArrowsRightLeftIcon className="h-5 w-5 text-zinc" />
                    </button>
                    <ColorPicker<FormData>
                      className="h-10 grow"
                      control={control}
                      name="color2"
                    />
                  </>
                )}
              </div>
            </label>
          )}

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.fadeSpeed')}: {toPercentage(values.fadeSpeed)}%
            </span>
            <Slider<FormData>
              className="w-full"
              control={control}
              name="fadeSpeed"
              min={0}
              max={255}
              step={5}
            />
          </label>

          {AnimationType.Sparkle === Number(values.type) && (
            <>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.sparkCount')}:{' '}
                  {toPercentage(values.animationSettings[7])}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  control={control}
                  name="animationSettings.7"
                  min={0}
                  max={255}
                  step={1}
                />
              </label>

              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.sparkFriction')}:{' '}
                  {toPercentage(values.animationSettings[8])}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  control={control}
                  name="animationSettings.8"
                  min={0}
                  max={255}
                  step={1}
                />
              </label>

              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.sparkFading')}:{' '}
                  {toPercentage(values.animationSettings[9])}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  control={control}
                  name="animationSettings.9"
                  min={0}
                  max={255}
                  step={1}
                />
              </label>

              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.sparkTail')}:{' '}
                  {toPercentage(values.animationSettings[10])}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  control={control}
                  name="animationSettings.10"
                  min={0}
                  max={255}
                  step={1}
                />
              </label>

              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.birthRate')}:{' '}
                  {toPercentage(values.animationSettings[11])}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  control={control}
                  name="animationSettings.11"
                  min={0}
                  max={255}
                  step={1}
                />
              </label>

              <label className="mb-6 flex flex-row justify-between">
                <span className="basis-1/2 self-center">
                  {t('zone.bounceAtCorner')}
                </span>
                <div className="basis-1/2 text-right">
                  <Switch<FormData>
                    control={control}
                    name="animationSettings.17"
                  />
                </div>
              </label>

              <Collapsible
                title={t('zone.variance')}
                className="mb-6"
                defaultOpen={defaultValues?.animationSettings?.some(
                  (value, index) =>
                    index >= 13 && index <= 17 && (value ?? 0) > 0,
                )}
              >
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.spawnVariance')}:{' '}
                    {toPercentage(values.animationSettings[12])}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.12"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.speedVariance')}:{' '}
                    {toPercentage(values.animationSettings[13])}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.13"
                    min={1}
                    max={255}
                    step={1}
                  />
                </label>

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.brightnessVariance')}:{' '}
                    {toPercentage(values.animationSettings[14])}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.14"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.frictionVariance')}:{' '}
                    {toPercentage(values.animationSettings[15])}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.15"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>

                <label className="flex flex-col">
                  <span className="mb-2">
                    {t('zone.fadingVariance')}:{' '}
                    {toPercentage(values.animationSettings[16])}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.16"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>
              </Collapsible>
            </>
          )}

          {AnimationType.Pulse === Number(values.type) && (
            <label className="flex flex-col">
              <span className="mb-2">
                {t('zone.pulseFading')}:{' '}
                {toPercentage(values.animationSettings[9])}%
              </span>
              <Slider<FormData>
                className="w-full"
                control={control}
                name="animationSettings.9"
                min={0}
                max={255}
                step={1}
              />
            </label>
          )}
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('zone.advanced')}
          </legend>

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.ledCount')}: {values.ledCount}
            </span>
            <Slider<FormData>
              className="w-full"
              control={control}
              name="ledCount"
              min={2}
              max={200}
              step={1}
            />
          </label>

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.ledVoltage')}: {values.ledVoltage}
            </span>
            <Slider<FormData>
              className="w-full"
              min={4.5}
              max={5.5}
              step={0.1}
              control={control}
              name="ledVoltage"
            />
          </label>
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.channelCurrents')}: {values.channelCurrents}
            </span>
            <Slider<FormData>
              className="w-full"
              min={1}
              max={200}
              step={1}
              control={control}
              name="channelCurrents"
            />
          </label>

          {AnimationType.Static !== Number(values.type) && (
            <label className="mb-6 flex flex-row">
              <div className="mr-4 self-center">
                <ArrowsRightLeftIcon className="h-4 w-4 text-zinc" />
              </div>
              <div className="flex flex-col">
                <span>{t('zone.direction.label')}</span>
                <span className="text-sm text-zinc">
                  {t('zone.direction.desc')}
                </span>
              </div>
              <div className="flex-grow self-center text-right">
                <Switch<FormData> control={control} name="reverse" />
              </div>
            </label>
          )}
        </fieldset>

        <Button type="submit" className="mb-4" disabled={isSubmitting}>
          {t('zone.submit')}
        </Button>

        <Button type="reset" onClick={onReset} disabled={isSubmitting}>
          {t('zone.reset')}
        </Button>
      </form>
    </>
  );
};

type ZoneProps = { routeId: string };

export const Zone = ({ routeId }: ZoneProps): JSX.Element => {
  const { reset } = useQueryErrorResetBoundary();
  const {
    route: { options },
  } = useMatch({
    from: routeId,
  });
  const zoneId = options.meta?.zoneId ?? 0;

  return (
    <ErrorBoundary FallbackComponent={Error} onReset={reset}>
      <Suspense fallback={<Loading />}>
        <Form zoneId={zoneId} />
      </Suspense>
    </ErrorBoundary>
  );
};
