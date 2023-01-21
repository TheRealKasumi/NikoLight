import { useQuery, UseQueryOptions } from '@tanstack/react-query';
import ky from 'ky';

const API_URL = '/api/info/system';

export type SystemInfo = {
  socInfo: {
    chipModel: string;
    chipRevision: number;
    cpuCores: number;
    cpuClock: number;
    freeHeap: number;
    flashSize: number;
    flashSpeed: number;
    sketchSize: number;
    freeSketchSpace: number;
  };
  tlSystemInfo: {
    rps: number;
    fps: number;
    ledCount: number;
  };
  hardwareInfo: {
    regulatorCount: number;
    regulatorVoltage: number;
    regulatorCurrentLimit: number;
    regulatorCurrentDraw: number;
    regulatorPowerLimit: number;
    regulatorPowerDraw: number;
    regulatorTemperature: number;
    fanSpeed: number;
    mpu6050: number;
    ds18b20: number;
    bh1750: number;
    audioUnit: number;
  };
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = SystemInfo & Response;

export const useSystemInfo = (
  options?: UseQueryOptions<DataResponse, Error, SystemInfo>,
) =>
  useQuery<DataResponse, Error, SystemInfo>({
    queryKey: [API_URL],
    queryFn: async () => await ky.get(API_URL).json(),
    select: (data) => ({
      hardwareInfo: data.hardwareInfo,
      socInfo: data.socInfo,
      tlSystemInfo: data.tlSystemInfo,
    }),
    ...options,
  });
