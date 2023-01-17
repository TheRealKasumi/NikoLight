import { ArrowsRightLeftIcon } from '@heroicons/react/24/outline';
import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { useMatch } from '@tanstack/react-router';
import hexRgb from 'hex-rgb';
import { Suspense } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import { useTranslation } from 'react-i18next';
import rgbHex from 'rgb-hex';
import {
  AnimationMode,
  AnimationType,
  Button,
  Collapsible,
  ColorPicker,
  Error,
  Loading,
  MotionSensorValue,
  Notification,
  Select,
  SelectItem,
  Slider,
  Switch,
  Toast,
} from '../components';
import { toPercentage } from '../libs';
import { useLed, useUpdateLed } from '../pages/api';

type FormData = {
  animationSettings: [
    string,
    number,
    number,
    number,
    number,
    number,
    number,
    string,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
    number,
  ];
  brightness: number;
  channelCurrents: number;
  color1: string;
  color2: string;
  fadeSpeed: number;
  ledCount: number;
  ledVoltage: number;
  offset: number;
  reverse: boolean;
  speed: number;
  type: string;
};

const DEFAULT_VALUES: FormData = {
  animationSettings: [...Array(25)].map((_, index) =>
    index === 0 || index === 7 ? '0' : 0,
  ) as FormData['animationSettings'],
  brightness: 50,
  channelCurrents: 16,
  color1: '#000000',
  color2: '#000000',
  fadeSpeed: 30,
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
  const { data } = useLed();
  const { mutateAsync, isSuccess, isError, error } = useUpdateLed();
  const zone = data?.[zoneId];

  const { handleSubmit, watch, control, reset, formState, setValue } =
    useForm<FormData>({
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
        fadeSpeed: zone?.fadeSpeed,
        reverse: zone?.reverse,
        channelCurrents: zone?.channelCurrents[0],
        ledCount: zone?.ledCount,
        ledVoltage: zone?.ledVoltage,
        offset: zone?.offset,
        speed: zone?.speed,
        type: zone?.type.toString(),
        animationSettings: [...Array(25)].map((_, index) =>
          index === 0 || index === 7
            ? zone?.animationSettings[index].toString()
            : zone?.animationSettings[index],
        ) as FormData['animationSettings'],
      },
      mode: 'onChange',
    });

  const onSubmit = handleSubmit(
    async ({
      animationSettings,
      channelCurrents,
      color1,
      color2,
      type,
      ...rest
    }) => {
      const led = JSON.parse(JSON.stringify(data)) as NonNullable<typeof data>;

      // Set animation settings
      led[zoneId].animationSettings = animationSettings.map(
        (animationSetting) => Number(animationSetting),
      );

      // Set color1
      const rgbColor1 = hexRgb(color1);
      led[zoneId].animationSettings[1] = rgbColor1.red;
      led[zoneId].animationSettings[2] = rgbColor1.green;
      led[zoneId].animationSettings[3] = rgbColor1.blue;

      // Set color2
      const rgbColor2 = hexRgb(color2);
      led[zoneId].animationSettings[4] = rgbColor2.red;
      led[zoneId].animationSettings[5] = rgbColor2.green;
      led[zoneId].animationSettings[6] = rgbColor2.blue;

      led[zoneId] = {
        ...led[zoneId],
        ...rest,
        channelCurrents: [...Array(3)].fill(channelCurrents),
        type: Number(type),
      };

      await mutateAsync(led);
    },
  );

  const onReset = async () => {
    reset(DEFAULT_VALUES);
    return await onSubmit();
  };

  const onTypeChange = (type: string) => {
    // Reset all animation settings
    setValue('animationSettings', DEFAULT_VALUES.animationSettings);
    setValue('color1', `#${rgbHex(0, 0, 0)}`);
    setValue('color2', `#${rgbHex(0, 0, 0)}`);

    switch (Number(type)) {
      case AnimationType.Rainbow:
        setValue(
          'animationSettings.0',
          AnimationMode[AnimationType.Rainbow].Solid.toString(),
        );
        break;

      case AnimationType.Sparkle:
        setValue(
          'animationSettings.0',
          AnimationMode[AnimationType.Sparkle].Center.toString(),
        );
        setValue('color1', `#${rgbHex(0, 0, 0)}`);
        setValue('animationSettings.8', 5);
        setValue('animationSettings.11', 229);
        setValue('animationSettings.12', 127);
        break;

      case AnimationType.Gradient:
        setValue(
          'animationSettings.0',
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
          'animationSettings.0',
          AnimationMode[AnimationType.ColorBar].LinearSmoothBorder.toString(),
        );
        setValue('color1', `#${rgbHex(255, 0, 0)}`);
        setValue('color2', `#${rgbHex(0, 0, 255)}`);
        break;

      case AnimationType.RainbowMotion:
        setValue('animationSettings.7', MotionSensorValue.ACC_X_G.toString());
        break;

      case AnimationType.GradientMotion:
        setValue('color1', `#${rgbHex(255, 0, 0)}`);
        setValue('color2', `#${rgbHex(0, 0, 255)}`);
        setValue('animationSettings.7', MotionSensorValue.ACC_X_G.toString());
        break;
    }
  };

  return (
    <>
      {isSuccess && <Toast title={t('zone.submitSuccessful')} />}

      {isError && <Notification message={error.message} />}

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
              <Select<FormData>
                control={control}
                name="type"
                onValueChange={onTypeChange}
              >
                {Object.entries(AnimationType)
                  .filter(([key]) => isNaN(Number(key)))
                  .filter(([, value]) => value !== AnimationType.FSEQ)
                  .map(([key, value]) => (
                    <SelectItem key={key} value={value.toString()}>
                      {t(`zone.animationTypes.${key}`)}
                    </SelectItem>
                  ))}
              </Select>
            </div>
          </label>

          {AnimationType.Static !== Number(watch('type')) && (
            <label className="mb-6 flex flex-row justify-between">
              <span className="basis-1/2 self-center">
                {t('zone.animationMode')}
              </span>
              <div className="basis-1/2 text-right">
                <Select<FormData> control={control} name="animationSettings.0">
                  {Object.entries(
                    AnimationMode[
                      Number(watch('type')) as keyof typeof AnimationMode
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

          {[AnimationType.RainbowMotion, AnimationType.GradientMotion].includes(
            Number(watch('type')),
          ) && (
            <label className="mb-6 flex flex-row justify-between">
              <span className="basis-1/2 self-center">
                {t('zone.sensorValue')}
              </span>
              <div className="basis-1/2 text-right">
                <Select<FormData> control={control} name="animationSettings.7">
                  {Object.entries(MotionSensorValue)
                    .filter(([key]) => isNaN(Number(key)))
                    .map(([key, value]) => (
                      <SelectItem key={key} value={value.toString()}>
                        {t(`zone.motionSensorValues.${key}`)}
                      </SelectItem>
                    ))}
                </Select>
              </div>
            </label>
          )}

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.brightness')} : {toPercentage(watch('brightness'))}%
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
            Number(watch('type')),
          ) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('zone.speed')}: {toPercentage(watch('speed'))}%
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

          {AnimationType.Static !== Number(watch('type')) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('zone.offset')}: {toPercentage(watch('offset'))}%
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
          ].includes(Number(watch('type'))) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">{t('zone.color1')}</span>
              <ColorPicker<FormData>
                className="h-10 w-full"
                control={control}
                name="color1"
              />
            </label>
          )}

          {[
            AnimationType.ColorBar,
            AnimationType.Gradient,
            AnimationType.GradientMotion,
          ].includes(Number(watch('type'))) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">{t('zone.color2')}</span>
              <ColorPicker<FormData>
                className="h-10 w-full"
                control={control}
                name="color2"
              />
            </label>
          )}

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.fadeSpeed')}: {toPercentage(watch('fadeSpeed'))}%
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

          {AnimationType.Sparkle === Number(watch('type')) && (
            <>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.sparkCount')}:{' '}
                  {toPercentage(watch('animationSettings.8'))}%
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
                  {t('zone.sparkFriction')}:{' '}
                  {toPercentage(watch('animationSettings.9'))}%
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
                  {t('zone.sparkFading')}:{' '}
                  {toPercentage(watch('animationSettings.10'))}%
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
                  {t('zone.sparkTail')}:{' '}
                  {toPercentage(watch('animationSettings.11'))}%
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

              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('zone.birthRate')}:{' '}
                  {toPercentage(watch('animationSettings.12'))}%
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

              <Collapsible
                title={t('zone.variance')}
                className="mb-6"
                defaultOpen={formState.defaultValues?.animationSettings?.some(
                  (value, index) =>
                    index >= 13 && index <= 17 && (value ?? 0) > 0,
                )}
              >
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.spawnVariance')}:{' '}
                    {toPercentage(watch('animationSettings.13'))}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.13"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.speedVariance')}:{' '}
                    {toPercentage(watch('animationSettings.14'))}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.14"
                    min={2}
                    max={200}
                    step={1}
                  />
                </label>

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.brightnessVariance')}:{' '}
                    {toPercentage(watch('animationSettings.15'))}%
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

                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('zone.frictionVariance')}:{' '}
                    {toPercentage(watch('animationSettings.16'))}%
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

                <label className="flex flex-col">
                  <span className="mb-2">
                    {t('zone.fadingVariance')}:{' '}
                    {toPercentage(watch('animationSettings.17'))}%
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    control={control}
                    name="animationSettings.17"
                    min={0}
                    max={255}
                    step={1}
                  />
                </label>
              </Collapsible>
            </>
          )}
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('zone.advanced')}
          </legend>

          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('zone.ledCount')}: {watch('ledCount')}
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
              {t('zone.ledVoltage')}: {watch('ledVoltage')}
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
              {t('zone.channelCurrents')}: {watch('channelCurrents')}
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

          {AnimationType.Static !== Number(watch('type')) && (
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

        <Button
          type="submit"
          className="mb-4"
          disabled={formState.isSubmitting}
        >
          {t('zone.submit')}
        </Button>

        <Button
          type="reset"
          onClick={onReset}
          disabled={formState.isSubmitting}
        >
          {t('zone.reset')}
        </Button>
      </form>
    </>
  );
};

type ZoneProps = { routeId: string };

export const Zone = ({ routeId }: ZoneProps): JSX.Element => {
  const { reset } = useQueryErrorResetBoundary();
  const { options } = useMatch(routeId);
  const zoneId = options.meta?.zoneId ?? 0;

  return (
    <ErrorBoundary FallbackComponent={Error} onReset={reset}>
      <Suspense fallback={<Loading />}>
        <Form zoneId={zoneId} />
      </Suspense>
    </ErrorBoundary>
  );
};
