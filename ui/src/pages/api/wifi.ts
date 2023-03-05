import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

export const WIFI_API_URL = '/api/config/wifi';

export type Wifi = {
  accessPointSsid: string;
  accessPointPassword: string;
  accessPointChannel: number;
  accessPointHidden: boolean;
  accessPointMaxConnections: number;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  wifiConfig: Wifi;
} & Response;

type Context = {
  cache?: DataResponse;
};

export const useWifi = (options?: UseQueryOptions<DataResponse, Error, Wifi>) =>
  useQuery<DataResponse, Error, Wifi>({
    queryKey: [WIFI_API_URL],
    queryFn: async () => await ky.get(WIFI_API_URL).json(),
    select: (data) => data.wifiConfig,
    ...options,
  });

export const useUpdateWifi = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Wifi, Context>({
    mutationFn: async (data) =>
      await ky.post(WIFI_API_URL, { json: { wifiConfig: data } }).json(),
    onMutate: async (wifiConfig) => {
      await queryClient.cancelQueries({ queryKey: [WIFI_API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([WIFI_API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([WIFI_API_URL], {
          ...cache,
          wifiConfig,
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataResponse>([WIFI_API_URL], context.cache);
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [WIFI_API_URL] });
    },
  });
};
