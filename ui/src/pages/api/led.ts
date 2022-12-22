import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

const API_URL = '/api/config/led';

export type Led = {
  brightness: number;
  customFields: number[];
  fadeSpeed: number;
  reverse: boolean;
  channelCurrents: number[];
  ledCount: number;
  ledPin: number;
  ledVoltage: number;
  offset: number;
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
    queryKey: [API_URL],
    queryFn: async () => await ky.get(API_URL).json(),
    select: (data) => data.ledConfig,
    ...options,
  });

export const useUpdateLed = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Led[], Context>({
    mutationFn: async (data) =>
      await ky
        .post(API_URL, {
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
      await queryClient.cancelQueries({ queryKey: [API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([API_URL], {
          ...cache,
          ledConfig,
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataResponse>([API_URL], context.cache);
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [API_URL] });
    },
  });
};
