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
  Animation,
  Button,
  ColorPicker,
  Error,
  Loading,
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

  const { handleSubmit, watch, control, reset, formState } = useForm<FormData>({
    defaultValues: {
      brightness: zone?.brightness,
      color1: `#${rgbHex(
        zone?.customFields[0] ?? 0,
        zone?.customFields[1] ?? 0,
        zone?.customFields[2] ?? 0,
      )}`,
      color2: `#${rgbHex(
        zone?.customFields[3] ?? 0,
        zone?.customFields[4] ?? 0,
        zone?.customFields[5] ?? 0,
      )}`,
      fadeSpeed: zone?.fadeSpeed,
      reverse: zone?.reverse,
      channelCurrents: zone?.channelCurrents[0],
      ledCount: zone?.ledCount,
      ledVoltage: zone?.ledVoltage,
      offset: zone?.offset,
      speed: zone?.speed,
      type: zone?.type.toString(),
    },
    mode: 'onChange',
  });

  const onSubmit = handleSubmit(
    async ({ color1, color2, channelCurrents, type, ...rest }) => {
      const led = JSON.parse(JSON.stringify(data)) as NonNullable<typeof data>;

      const rgbColor1 = hexRgb(color1);
      led[zoneId].customFields[0] = rgbColor1.red;
      led[zoneId].customFields[1] = rgbColor1.green;
      led[zoneId].customFields[2] = rgbColor1.blue;

      const rgbColor2 = hexRgb(color2);
      led[zoneId].customFields[3] = rgbColor2.red;
      led[zoneId].customFields[4] = rgbColor2.green;
      led[zoneId].customFields[5] = rgbColor2.blue;

      led[zoneId] = {
        ...led[zoneId],
        ...rest,
        channelCurrents: [...Array(3)].fill(channelCurrents),
        type: Number(type),
      };

      return await mutateAsync(led);
    },
  );

  const onReset = async () => {
    reset(DEFAULT_VALUES);
    return await onSubmit();
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
            <span className="basis-1/2 self-center">{t('zone.animation')}</span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="type">
                {Object.keys(Animation)
                  .filter((key) => isNaN(Number(key)))
                  .filter((key) => key !== Animation[Animation.Custom])
                  .map((key, index) => (
                    <SelectItem key={key} value={index.toString()}>
                      {t(`zone.animationTypes.${key}`)}
                    </SelectItem>
                  ))}
              </Select>
            </div>
          </label>

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

          {![
            Animation.GradientCenter,
            Animation.GradientLinear,
            Animation.Static,
          ].includes(Number(watch('type'))) && (
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

          {Animation.Static !== Number(watch('type')) && (
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
            Animation.ColorBarsCenterHard,
            Animation.ColorBarsCenterSoft,
            Animation.ColorBarsLinearHard,
            Animation.ColorBarsLinearSoft,
            Animation.GradientCenter,
            Animation.GradientCenteredAccX,
            Animation.GradientCenteredAccY,
            Animation.GradientLinear,
            Animation.GradientLinearAccX,
            Animation.GradientLinearAccY,
            Animation.Static,
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
            Animation.ColorBarsCenterHard,
            Animation.ColorBarsCenterSoft,
            Animation.ColorBarsLinearHard,
            Animation.ColorBarsLinearSoft,
            Animation.GradientCenter,
            Animation.GradientCenteredAccX,
            Animation.GradientCenteredAccY,
            Animation.GradientLinear,
            Animation.GradientLinearAccX,
            Animation.GradientLinearAccY,
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

          {Animation.Static !== Number(watch('type')) && (
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
