import { useMutation } from '@tanstack/react-query';
import ky from 'ky';

export const MOTION_SENSOR_CALIBRATION_API_URL =
  '/api/config/motion/calibration';

export type MotionSensorCalibration = {
  accXRaw: number;
  accYRaw: number;
  accZRaw: number;
  gyroXRaw: number;
  gyroYRaw: number;
  gyroZRaw: number;
  accXG: number;
  accYG: number;
  accZG: number;
  gyroXDeg: number;
  gyroYDeg: number;
  gyroZDeg: number;
};

type Response = {
  status: number;
  message: string;
};

export const useAutoMotionSensorCalibration = () =>
  useMutation<Response, Error>({
    mutationFn: async () =>
      await ky.patch(`${MOTION_SENSOR_CALIBRATION_API_URL}/auto`).json(),
  });
