import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

export const LED_API_URL = '/api/config/led';

export type Led = {
  animationSettings: number[];
  brightness: number;
  channelCurrents: number[];
  dataSource: number;
  fadeSpeed: number;
  ledCount: number;
  ledPin: number;
  ledVoltage: number;
  offset: number;
  reverse: boolean;
  speed: number;
  type: number;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  ledConfig: Led[];
} & Response;

type Context = {
  cache?: DataResponse;
};

export const useLed = (options?: UseQueryOptions<DataResponse, Error, Led[]>) =>
  useQuery<DataResponse, Error, Led[]>({
    queryKey: [LED_API_URL],
    queryFn: async () => await ky.get(LED_API_URL).json(),
    select: (data) => data.ledConfig,
    ...options,
  });

export const useUpdateLed = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Led[], Context>({
    mutationFn: async (data) =>
      await ky
        .post(LED_API_URL, {
          json: { ledConfig: data },
          throwHttpErrors: false,
          hooks: {
            afterResponse: [
              async (_input, _options, response) => {
                const body: Response = await response.json();
                if (!response.ok) {
                  // Custom error message handling
                  throw new Error(body.message);
                }
              },
            ],
          },
        })
        .json(),
    onMutate: async (ledConfig) => {
      await queryClient.cancelQueries({ queryKey: [LED_API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([LED_API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([LED_API_URL], {
          ...cache,
          ledConfig,
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataResponse>([LED_API_URL], context.cache);
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [LED_API_URL] });
    },
  });
};
