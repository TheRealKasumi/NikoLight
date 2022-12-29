import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

const API_URL = '/api/config/ui';

export type Ui = {
  expertMode: boolean;
  firmware: string;
  language: string;
  theme: string;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  uiConfig: Ui;
} & Response;

type Context = {
  cache?: DataResponse;
};

export const useUi = (options?: UseQueryOptions<DataResponse, Error, Ui>) =>
  useQuery<DataResponse, Error, Ui>({
    queryKey: [API_URL],
    queryFn: async () => await ky.get(API_URL).json(),
    select: (data) => data.uiConfig,
    ...options,
  });

export const useUpdateUi = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Ui, Context>({
    mutationFn: async (data) =>
      await ky.post(API_URL, { json: { uiConfig: data } }).json(),
    onMutate: async (uiConfig) => {
      await queryClient.cancelQueries({ queryKey: [API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([API_URL], {
          ...cache,
          uiConfig,
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
