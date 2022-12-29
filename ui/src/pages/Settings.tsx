import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { Suspense } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import {
  Button,
  Error,
  InputPassword,
  InputText,
  Loading,
  Notification,
  Select,
  SelectItem,
  Slider,
  Toast,
} from '../components';
import { useSystem, useUpdateSystem, useUpdateWifi, useWifi } from './api';

enum LightSensorMode {
  AlwaysOff,
  AlwaysOn,
  AutomaticOnOffADC,
  AutomaticBrightnessADC,
  AutomaticOnOffBH1750,
  AutomaticBrightnessBH1750,
  AutomaticOnOffMPU6050,
}

type FormData = {
  system: {
    fanMaxPwmValue: number;
    fanMaxTemperature: number;
    fanMinPwmValue: number;
    fanMinTemperature: number;
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
};

const DEFAULT_VALUES: FormData = {
  system: {
    fanMaxPwmValue: 255,
    fanMaxTemperature: 80,
    fanMinPwmValue: 75,
    fanMinTemperature: 60,
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
};

const Form = (): JSX.Element => {
  const { data: system } = useSystem();
  const { data: wifi } = useWifi();
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

  const { handleSubmit, watch, control, reset, formState } = useForm<FormData>({
    defaultValues: {
      system: {
        fanMaxPwmValue: system?.fanMaxPwmValue,
        fanMaxTemperature: system?.fanMaxTemperature,
        fanMinPwmValue: system?.fanMinPwmValue,
        fanMinTemperature: system?.fanMinTemperature,
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
    },
    mode: 'onChange',
  });

  const onSubmit = handleSubmit(async (data) => {
    const wifiCopy = JSON.parse(JSON.stringify(wifi)) as NonNullable<
      typeof wifi
    >;
    const systemCopy = JSON.parse(JSON.stringify(system)) as NonNullable<
      typeof system
    >;
    const { lightSensorMode, logLevel, ...rest } = data.system;

    return Promise.all([
      mutateAsyncWifi({ ...wifiCopy, ...data.wifi }),
      mutateAsyncSystem({
        ...systemCopy,
        ...rest,
        lightSensorMode: Number(lightSensorMode),
        logLevel: Number(logLevel),
      }),
    ]);
  });

  const onReset = async () => {
    reset(DEFAULT_VALUES);
    return await onSubmit();
  };

  return (
    <>
      {isSystemSuccess && isWifiSuccess && (
        <Toast title="Saved successfully!" />
      )}

      {isSystemError && <Notification message={systemError.message} />}
      {isWifiError && <Notification message={wifiError.message} />}

      <form onSubmit={onSubmit}>
        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">Light Sensor</legend>
          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">Mode</span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="system.lightSensorMode">
                <SelectItem value={LightSensorMode.AlwaysOff.toString()}>
                  Always Off
                </SelectItem>
                <SelectItem value={LightSensorMode.AlwaysOn.toString()}>
                  Always On
                </SelectItem>
                <SelectItem
                  value={LightSensorMode.AutomaticOnOffADC.toString()}
                >
                  Automatic On/Off ADC
                </SelectItem>
                <SelectItem
                  value={LightSensorMode.AutomaticBrightnessADC.toString()}
                >
                  Automatic Brightness ADC
                </SelectItem>
                <SelectItem
                  value={LightSensorMode.AutomaticOnOffBH1750.toString()}
                >
                  Automatic On/Off BH1750
                </SelectItem>
                <SelectItem
                  value={LightSensorMode.AutomaticBrightnessBH1750.toString()}
                >
                  Automatic Brightness BH1750
                </SelectItem>
                <SelectItem
                  value={LightSensorMode.AutomaticOnOffMPU6050.toString()}
                >
                  Automatic On/Off MPU6050
                </SelectItem>
              </Select>
            </div>
          </label>
          {![LightSensorMode.AlwaysOff, LightSensorMode.AlwaysOn].includes(
            Number(watch('system.lightSensorMode')),
          ) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                Threshold: {watch('system.lightSensorThreshold')}
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
          ].includes(Number(watch('system.lightSensorMode'))) && (
            <>
              <label className="mb-6 flex flex-col">
                <span className="mb-2">
                  Minimum Brightness:{' '}
                  {watch('system.lightSensorMinAmbientBrightness')}
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
                  Maximum Brightness:{' '}
                  {watch('system.lightSensorMaxAmbientBrightness')}
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
                  Minimum LED Brightness:{' '}
                  {watch('system.lightSensorMinLedBrightness')}
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
                  Maximum LED Brightness:{' '}
                  {watch('system.lightSensorMaxLedBrightness')}
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
            Number(watch('system.lightSensorMode')) && (
            <label className="mb-6 flex flex-col">
              <span className="mb-2">
                Duration: {watch('system.lightSensorDuration')}
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
          <legend className="mb-6 text-lg font-medium">Wifi Hotspot</legend>
          <label className="mb-6 flex flex-row">
            <span className="basis-1/2 self-center">SSID</span>
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
            <span className="basis-1/2 self-center">Password</span>
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
          <legend className="mb-6 text-lg font-medium">Regulator</legend>
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              Power Limit (W): {watch('system.regulatorPowerLimit')}
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
              Throttle Temperature (°C):{' '}
              {watch('system.regulatorHighTemperature')}
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
              Shut Down Temperature (°C):{' '}
              {watch('system.regulatorCutoffTemperature')}
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
          <label className="mb-6 flex flex-col">
            <span className="mb-2">
              Fan Start Temp (°C): {watch('system.fanMinTemperature')}
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
              Fan Full Speed Temp (°C): {watch('system.fanMaxTemperature')}
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
              Fan Min PWM (Stall Guard): {watch('system.fanMinPwmValue')}
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
              Fan Max PWM: {watch('system.fanMaxPwmValue')}
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
        </fieldset>

        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            Logging and Debugging
          </legend>
          <label className="mb-6 flex flex-row justify-between">
            <span className="basis-1/2 self-center">Log Level</span>
            <div className="basis-1/2 text-right">
              <Select<FormData> control={control} name="system.logLevel">
                <SelectItem value="0">Debug</SelectItem>
                <SelectItem value="1">Info</SelectItem>
                <SelectItem value="2">Warning</SelectItem>
                <SelectItem value="3">Error</SelectItem>
              </Select>
            </div>
          </label>
        </fieldset>

        <Button
          type="submit"
          className="mb-4"
          disabled={formState.isSubmitting}
        >
          Apply Settings
        </Button>

        <Button
          type="reset"
          onClick={onReset}
          disabled={formState.isSubmitting}
        >
          Reset to default
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
