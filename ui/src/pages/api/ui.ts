import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

export const UI_API_URL = '/api/config/ui';

export type Ui = {
  expertMode: boolean;
  firmware: string;
  language: string;
  theme: string;
};

type UiResponse = {
  status: number;
  message: string;
};

export type UiDataResponse = {
  uiConfig: Ui;
} & UiResponse;

type UiContext = {
  cache?: UiDataResponse;
};

export const useUi = (options?: UseQueryOptions<UiDataResponse, Error, Ui>) =>
  useQuery<UiDataResponse, Error, Ui>({
    queryKey: [UI_API_URL],
    queryFn: async () => await ky.get(UI_API_URL).json(),
    select: (data) => data.uiConfig,
    ...options,
  });

export const useUpdateUi = () => {
  const queryClient = useQueryClient();
  return useMutation<UiResponse, Error, Ui, UiContext>({
    mutationFn: async (data) =>
      await ky.patch(UI_API_URL, { json: { uiConfig: data } }).json(),
    onMutate: async (uiConfig) => {
      await queryClient.cancelQueries({ queryKey: [UI_API_URL] });
      const cache = queryClient.getQueryData<UiDataResponse>([UI_API_URL]);

      if (cache) {
        queryClient.setQueryData<UiDataResponse>([UI_API_URL], {
          ...cache,
          uiConfig,
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<UiDataResponse>([UI_API_URL], context.cache);
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [UI_API_URL] });
    },
  });
};
