import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { Suspense, type MouseEvent } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import { useTranslation } from 'react-i18next';
import {
  AlertDialog,
  Button,
  Error,
  InputPassword,
  InputText,
  Loading,
  Notification,
  Select,
  SelectItem,
  Slider,
  Switch,
  Toast,
} from '../components';
import i18n from '../i18n';
import { changeTheme, toPercentage } from '../libs';
import {
  useAutoMotionSensorCalibration,
  useSystem,
  useSystemInfo,
  useUi,
  useUpdateSystem,
  useUpdateUi,
  useUpdateWifi,
  useWifi,
  type System,
  type Ui,
  type Wifi,
} from './api';

enum LightSensorMode {
  AlwaysOff,
  AlwaysOn,
  AutomaticOnOffADC,
  AutomaticBrightnessADC,
  AutomaticOnOffBH1750,
  AutomaticBrightnessBH1750,
  AutomaticOnOffMPU6050,
}

enum FanMode {
  Automatic,
  ManualOff,
  Manual25,
  Manual50,
  Manual75,
  Manual100,
}

type FormData = {
  system: {
    fanMaxPwmValue: number;
    fanMaxTemperature: number;
    fanMinPwmValue: number;
    fanMinTemperature: number;
    fanMode: string;
    lightSensorDuration: number;
    lightSensorMaxAmbientBrightness: number;
    lightSensorMaxLedBrightness: number;
    lightSensorMinAmbientBrightness: number;
    lightSensorMinLedBrightness: number;
    lightSensorMode: string;
    lightSensorThreshold: number;
    logLevel: string;
    regulatorCutoffTemperature: number;
    regulatorHighTemperature: number;
    regulatorPowerLimit: number;
  };
  wifi: {
    accessPointPassword: string;
    accessPointSsid: string;
  };
  ui: {
    expertMode: boolean;
    language: string;
    theme: string;
  };
};

const DEFAULT_VALUES: FormData = {
  system: {
    fanMaxPwmValue: 255,
    fanMaxTemperature: 80,
    fanMinPwmValue: 75,
    fanMinTemperature: 60,
    fanMode: '0',
    lightSensorDuration: 6,
    lightSensorMaxAmbientBrightness: 255,
    lightSensorMaxLedBrightness: 255,
    lightSensorMinAmbientBrightness: 5,
    lightSensorMinLedBrightness: 0,
    lightSensorMode: '1',
    lightSensorThreshold: 5,
    logLevel: '1',
    regulatorCutoffTemperature: 85,
    regulatorHighTemperature: 70,
    regulatorPowerLimit: 24,
  },
  wifi: {
    accessPointPassword: 'TesLightPW',
    accessPointSsid: 'TesLight',
  },
  ui: {
    expertMode: false,
    language: 'en',
    theme: 'dark',
  },
};

const Form = (): JSX.Element => {
  const { t } = useTranslation();
  const { data: system } = useSystem();
  const { data: systemInfo, refetch: refetchSystemInfo } = useSystemInfo();
  const { data: wifi } = useWifi();
  const { data: ui } = useUi();
  const {
    mutateAsync: mutateAsyncSystem,
    isSuccess: isSystemSuccess,
    isError: isSystemError,
    error: systemError,
  } = useUpdateSystem();
  const {
    mutateAsync: mutateAsyncWifi,
    isSuccess: isWifiSuccess,
    isError: isWifiError,
    error: wifiError,
  } = useUpdateWifi();
  const {
    mutateAsync: mutateAsyncUi,
    isSuccess: isUiSuccess,
    isError: isUiError,
    error: uiError,
  } = useUpdateUi();
  const {
    mutateAsync: mutateAsyncAutoMotionSensorCalibration,
    isSuccess: isAutoMotionSensorCalibrationSuccess,
    isLoading: isAutoMotionSensorCalibrationLoading,
    isError: isAutoMotionSensorCalibrationError,
    error: autoMotionSensorCalibrationError,
  } = useAutoMotionSensorCalibration();

  const {
    handleSubmit,
    watch,
    control,
    reset,
    formState: { dirtyFields, isSubmitting },
  } = useForm<FormData>({
    defaultValues: {
      system: {
        fanMaxPwmValue: system?.fanMaxPwmValue,
        fanMaxTemperature: system?.fanMaxTemperature,
        fanMinPwmValue: system?.fanMinPwmValue,
        fanMinTemperature: system?.fanMinTemperature,
        fanMode: system?.fanMode.toString(),
        lightSensorDuration: system?.lightSensorDuration,
        lightSensorMaxAmbientBrightness:
          system?.lightSensorMaxAmbientBrightness,
        lightSensorMaxLedBrightness: system?.lightSensorMaxLedBrightness,
        lightSensorMinAmbientBrightness:
          system?.lightSensorMinAmbientBrightness,
        lightSensorMinLedBrightness: system?.lightSensorMinLedBrightness,
        lightSensorMode: system?.lightSensorMode.toString(),
        lightSensorThreshold: system?.lightSensorThreshold,
        logLevel: system?.logLevel.toString(),
        regulatorCutoffTemperature: system?.regulatorCutoffTemperature,
        regulatorHighTemperature: system?.regulatorHighTemperature,
        regulatorPowerLimit: system?.regulatorPowerLimit,
      },
      wifi: {
        accessPointPassword: wifi?.accessPointPassword,
        accessPointSsid: wifi?.accessPointSsid,
      },
      ui: {
        expertMode: ui?.expertMode,
        language: ui?.language,
        theme: ui?.theme,
      },
    },
    mode: 'onChange',
  });

  const onSubmit = handleSubmit(async (data, event) => {
    const wifiCopy: Wifi = JSON.parse(JSON.stringify(wifi));
    const uiCopy: Ui = JSON.parse(JSON.stringify(ui));
    const systemCopy: System = JSON.parse(JSON.stringify(system));
    const { fanMode, lightSensorMode, logLevel, ...rest } = data.system;

    await mutateAsyncUi(
      { ...uiCopy, ...data.ui },
      {
        onSuccess: (_data, variables) => {
          changeTheme(variables.theme);
          i18n.changeLanguage(variables.language);
        },
      },
    );

    await mutateAsyncSystem(
      {
        ...systemCopy,
        ...rest,
        fanMode: Number(fanMode),
        lightSensorMode: Number(lightSensorMode),
        logLevel: Number(logLevel),
      },
      {
        onSuccess: async () => {
          await refetchSystemInfo();
        },
      },
    );

    // Update WiFi configuration only if there was a change.
    if (
      dirtyFields.wifi ||
      (event?.target.type === 'reset' &&
        (DEFAULT_VALUES.wifi.accessPointSsid !== wifi?.accessPointSsid ||
          DEFAULT_VALUES.wifi.accessPointPassword !==
            wifi?.accessPointPassword))
    ) {
      await mutateAsyncWifi({ ...wifiCopy, ...data.wifi });
    }

    // Reset dirty fields
    reset({}, { keepValues: true });
  });

  const onReset = async (event: MouseEvent<HTMLButtonElement>) => {
    reset(DEFAULT_VALUES);
    return await onSubmit(event);
  };

  const getAvailablePowerModes = () => {
    const hasMPU6050 = (systemInfo?.hardwareInfo.mpu6050 ?? 0) > 0;
    const hasBH1750 = (systemInfo?.hardwareInfo.bh1750 ?? 0) > 0;

    return Object.entries(LightSensorMode)
      .filter(([key]) => isNaN(Number(key)))
      .filter(
        ([, value]) =>
          value !== LightSensorMode.AutomaticOnOffMPU6050 ||
          (value === LightSensorMode.AutomaticOnOffMPU6050 && hasMPU6050),
      )
      .filter(
        ([, value]) =>
          value !== LightSensorMode.AutomaticBrightnessBH1750 ||
          (value === LightSensorMode.AutomaticBrightnessBH1750 && hasBH1750),
      )
      .filter(
        ([, value]) =>
          value !== LightSensorMode.AutomaticOnOffBH1750 ||
          (value === LightSensorMode.AutomaticOnOffBH1750 && hasBH1750),
      );
  };

  const hasTemperatureSensors = (systemInfo?.hardwareInfo.ds18b20 ?? 0) > 0;

  const values = watch();

  return (
    <>
      {isSystemSuccess &&
        isUiSuccess &&
        (isWifiSuccess || !dirtyFields.wifi) && (
          <Toast title={t('settings.submitSuccessful')} />
        )}

      {isAutoMotionSensorCalibrationSuccess && (
        <Toast title={t('settings.motionSensorAutoCalibrationSuccessful')} />
      )}

      {isSystemError && (
        <Notification state="error" message={systemError.message} />
      )}

      {isWifiError && (
        <Notification state="error" message={wifiError.message} />
      )}

      {isUiError && <Notification state="error" message={uiError.message} />}

      {isAutoMotionSensorCalibrationError && (
        <Notification
          state="error"
          message={autoMotionSensorCalibrationError.message}
        />
      )}

      {isAutoMotionSensorCalibrationLoading && <Loading overlay />}

      {Number(values.system.fanMode) === FanMode.Automatic &&
        !hasTemperatureSensors && (
          <Notification
            message={t('settings.fanModeAutomaticWithoutTemperatureSensors')}
            state="warning"
          />
        )}

      <form onSubmit={onSubmit}>
        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('settings.power')}
          </legend>
          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/3 self-center">
              {t('settings.lightSensorMode')}
            </span>
            <div className="basis-2/2 text-right">
              <Select<FormData> control={control} name="system.lightSensorMode">
                {getAvailablePowerModes().map(([key, value]) => (
                  <SelectItem key={key} value={value.toString()}>
                    {t(`settings.lightSensorModes.${key}`)}
                  </SelectItem>
                ))}
              </Select>
            </div>
          </label>
          {![LightSensorMode.AlwaysOff, LightSensorMode.AlwaysOn].includes(
            Number(values.system.lightSensorMode),
          ) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('settings.lightSensorThreshold')}:{' '}
                {toPercentage(values.system.lightSensorThreshold)}%
              </span>
              <Slider<FormData>
                className="w-full"
                min={1}
                max={255}
                step={1}
                control={control}
                name="system.lightSensorThreshold"
              />
            </label>
          )}
          {[
            LightSensorMode.AutomaticBrightnessADC,
            LightSensorMode.AutomaticBrightnessBH1750,
          ].includes(Number(values.system.lightSensorMode)) && (
            <>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('settings.lightSensorMinAmbientBrightness')}:{' '}
                  {toPercentage(values.system.lightSensorMinAmbientBrightness)}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  min={1}
                  max={255}
                  step={1}
                  control={control}
                  name="system.lightSensorMinAmbientBrightness"
                />
              </label>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('settings.lightSensorMaxAmbientBrightness')}:{' '}
                  {toPercentage(values.system.lightSensorMaxAmbientBrightness)}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  min={1}
                  max={255}
                  step={1}
                  control={control}
                  name="system.lightSensorMaxAmbientBrightness"
                />
              </label>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('settings.lightSensorMinLedBrightness')}:{' '}
                  {toPercentage(values.system.lightSensorMinLedBrightness)}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  min={0}
                  max={255}
                  step={1}
                  control={control}
                  name="system.lightSensorMinLedBrightness"
                />
              </label>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  {t('settings.lightSensorMaxLedBrightness')}:{' '}
                  {toPercentage(values.system.lightSensorMaxLedBrightness)}%
                </span>
                <Slider<FormData>
                  className="w-full"
                  min={1}
                  max={255}
                  step={1}
                  control={control}
                  name="system.lightSensorMaxLedBrightness"
                />
              </label>
            </>
          )}
          {LightSensorMode.AutomaticOnOffMPU6050 ===
            Number(values.system.lightSensorMode) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                {t('settings.lightSensorDuration')}:{' '}
                {values.system.lightSensorDuration * 5}s
              </span>
              <Slider<FormData>
                className="w-full"
                min={1}
                max={255}
                step={1}
                control={control}
                name="system.lightSensorDuration"
              />
            </label>
          )}
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('settings.wifiHotspot')}
          </legend>
          <label className="mb-6 flex flex-row">
            <span className="basis-1/2 self-center">
              {t('settings.accessPointSsid')}
            </span>
            <div className="basis-1/2">
              <InputText<FormData>
                control={control}
                maxLength={32}
                minLength={2}
                name="wifi.accessPointSsid"
                pattern="^[0-9a-zA-Z-_ ]+$"
                placeholder="TesLight"
              />
            </div>
          </label>
          <label className="mb-6 flex flex-row">
            <span className="basis-1/2 self-center">
              {t('settings.accessPointPassword')}
            </span>
            <div className="basis-1/2">
              <InputPassword<FormData>
                control={control}
                maxLength={63}
                minLength={8}
                name="wifi.accessPointPassword"
                pattern='^[0-9a-zA-Z+-_!"§$%&\/()=?*# ]+$'
                placeholder="7e5L19H7"
              />
            </div>
          </label>
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('settings.regulator')}
          </legend>
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('settings.regulatorPowerLimit')}:{' '}
              {values.system.regulatorPowerLimit}
            </span>
            <Slider<FormData>
              className="w-full"
              min={1}
              max={30}
              step={1}
              control={control}
              name="system.regulatorPowerLimit"
            />
          </label>
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('settings.regulatorHighTemperature')}:{' '}
              {values.system.regulatorHighTemperature}
            </span>
            <Slider<FormData>
              className="w-full"
              min={60}
              max={85}
              step={1}
              control={control}
              name="system.regulatorHighTemperature"
            />
          </label>
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              {t('settings.regulatorCutoffTemperature')}:{' '}
              {values.system.regulatorCutoffTemperature}
            </span>
            <Slider<FormData>
              className="w-full"
              min={70}
              max={95}
              step={1}
              control={control}
              name="system.regulatorCutoffTemperature"
            />
          </label>
          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">
              {t('settings.fanMode')}
            </span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="system.fanMode">
                {Object.entries(FanMode)
                  .filter(([key]) => isNaN(Number(key)))
                  .map(([key, value]) => (
                    <SelectItem key={key} value={value.toString()}>
                      {t(`settings.fanModes.${key}`)}
                    </SelectItem>
                  ))}
              </Select>
            </div>
          </label>
          {FanMode.Automatic === Number(values.system.fanMode) &&
            hasTemperatureSensors && (
              <>
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('settings.fanMinTemperature')}:{' '}
                    {values.system.fanMinTemperature}
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    min={45}
                    max={70}
                    step={1}
                    control={control}
                    name="system.fanMinTemperature"
                  />
                </label>
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('settings.fanMaxTemperature')}:{' '}
                    {values.system.fanMaxTemperature}
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    min={60}
                    max={90}
                    step={1}
                    control={control}
                    name="system.fanMaxTemperature"
                  />
                </label>
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('settings.fanMinPwmValue')}:{' '}
                    {values.system.fanMinPwmValue}
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    min={0}
                    max={255}
                    step={1}
                    control={control}
                    name="system.fanMinPwmValue"
                  />
                </label>
                <label className="mb-6 flex flex-col">
                  <span className="mb-2">
                    {t('settings.fanMaxPwmValue')}:{' '}
                    {values.system.fanMaxPwmValue}
                  </span>
                  <Slider<FormData>
                    className="w-full"
                    min={0}
                    max={255}
                    step={1}
                    control={control}
                    name="system.fanMaxPwmValue"
                  />
                </label>
              </>
            )}
        </fieldset>

        {(systemInfo?.hardwareInfo.mpu6050 ?? 0) > 0 && (
          <fieldset className="mb-6">
            <legend className="mb-6 text-lg font-medium">
              {t('settings.motionSensor')}
            </legend>

            <label className="mb-6 flex flex-row justify-between">
              <span className="basis-1/2 self-center">
                {t('settings.motionSensorCalibration')}
              </span>
              <div className="basis-1/2 text-right">
                <AlertDialog
                  title={t('settings.motionSensorAutoCalibrationTitle')}
                  description={t(
                    'settings.motionSensorAutoCalibrationDescription',
                  )}
                  onConfirm={async () => {
                    await mutateAsyncAutoMotionSensorCalibration();
                  }}
                >
                  <Button
                    className="w-fit border-0 bg-cloud text-sm font-medium text-sky dark:bg-gray"
                    variant="secondary"
                  >
                    {t('settings.motionSensorAutoCalibration')}
                  </Button>
                </AlertDialog>
              </div>
            </label>
          </fieldset>
        )}

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('settings.ui')}
          </legend>

          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">
              {t('settings.language')}
            </span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="ui.language">
                <SelectItem value="de">Deutsch</SelectItem>
                <SelectItem value="en">English</SelectItem>
              </Select>
            </div>
          </label>

          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">{t('settings.theme')}</span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="ui.theme">
                <SelectItem value="dark">
                  {t('settings.themes.dark')}
                </SelectItem>
                <SelectItem value="light">
                  {t('settings.themes.light')}
                </SelectItem>
              </Select>
            </div>
          </label>

          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">
              {t('settings.expertMode')}
            </span>
            <div className="basis-1/2 text-right">
              <Switch<FormData> control={control} name="ui.expertMode" />
            </div>
          </label>
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('settings.loggingAndDebugging')}
          </legend>
          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">
              {t('settings.logLevel')}
            </span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="system.logLevel">
                <SelectItem value="0">
                  {t('settings.logLevels.debug')}
                </SelectItem>
                <SelectItem value="1">
                  {t('settings.logLevels.info')}
                </SelectItem>
                <SelectItem value="2">
                  {t('settings.logLevels.warning')}
                </SelectItem>
                <SelectItem value="3">
                  {t('settings.logLevels.error')}
                </SelectItem>
              </Select>
            </div>
          </label>
        </fieldset>

        <Button type="submit" className="mb-4" disabled={isSubmitting}>
          {t('settings.submit')}
        </Button>

        <Button type="reset" onClick={onReset} disabled={isSubmitting}>
          {t('settings.reset')}
        </Button>
      </form>
    </>
  );
};

export const Settings = (): JSX.Element => {
  const { reset } = useQueryErrorResetBoundary();

  return (
    <ErrorBoundary FallbackComponent={Error} onReset={reset}>
      <Suspense fallback={<Loading />}>
        <Form />
      </Suspense>
    </ErrorBoundary>
  );
};
