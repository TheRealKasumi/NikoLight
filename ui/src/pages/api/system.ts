import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

export const SYSTEM_API_URL = '/api/config/system';

export type System = {
  fanMaxPwmValue: number;
  fanMaxTemperature: number;
  fanMinPwmValue: number;
  fanMinTemperature: number;
  fanMode: number;
  lightSensorDuration: number;
  lightSensorMaxAmbientBrightness: number;
  lightSensorMaxLedBrightness: number;
  lightSensorMinAmbientBrightness: number;
  lightSensorMinLedBrightness: number;
  lightSensorMode: number;
  lightSensorThreshold: number;
  logLevel: number;
  regulatorCutoffTemperature: number;
  regulatorHighTemperature: number;
  regulatorPowerLimit: number;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  systemConfig: System;
} & Response;

type Context = {
  cache?: DataResponse;
};

export const useSystem = (
  options?: UseQueryOptions<DataResponse, Error, System>,
) =>
  useQuery<DataResponse, Error, System>({
    queryKey: [SYSTEM_API_URL],
    queryFn: async () => await ky.get(SYSTEM_API_URL).json(),
    select: (data) => data.systemConfig,
    ...options,
  });

export const useUpdateSystem = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, System, Context>({
    mutationFn: async (data) =>
      await ky.patch(SYSTEM_API_URL, { json: { systemConfig: data } }).json(),
    onMutate: async (systemConfig) => {
      await queryClient.cancelQueries({ queryKey: [SYSTEM_API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([SYSTEM_API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([SYSTEM_API_URL], {
          ...cache,
          systemConfig,
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataResponse>([SYSTEM_API_URL], context.cache);
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [SYSTEM_API_URL] });
    },
  });
};
