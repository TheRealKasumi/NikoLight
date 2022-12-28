import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

const API_URL = '/api/fseq';

export type Fseq = {
  fileName: string;
  fileSize: number;
  fileId: number;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  fileList: Fseq[];
} & Response;

type Context = {
  cache?: DataResponse;
};

export const useFseq = (
  options?: UseQueryOptions<DataResponse, Error, Fseq[]>,
) =>
  useQuery<DataResponse, Error, Fseq[]>({
    queryKey: [API_URL],
    queryFn: async () => await ky.get(API_URL).json(),
    select: (data) => data.fileList,
    ...options,
  });

export const useUpdateFseq = () => {
  const queryClient = useQueryClient();
  return useMutation<
    Response,
    Error,
    { file: File; fileName: string },
    Context
  >({
    mutationFn: async (data) => {
      const formData = new FormData();
      formData.append('file', data.file);
      return await ky
        .post(API_URL, {
          body: formData,
          searchParams: { fileName: data.fileName },
          timeout: false,
        })
        .json();
    },
    onSuccess: () => {
      queryClient.refetchQueries({ queryKey: [API_URL] });
    },
  });
};

export const useDeleteFseq = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, { fileName: string }, Context>({
    mutationFn: async (data) =>
      await ky
        .delete(API_URL, {
          searchParams: { fileName: data.fileName },
        })
        .json(),
    onMutate: async (data) => {
      await queryClient.cancelQueries({ queryKey: [API_URL] });
      const cache = queryClient.getQueryData<DataResponse>([API_URL]);

      if (cache) {
        queryClient.setQueryData<DataResponse>([API_URL], {
          ...cache,
          fileList: cache.fileList.filter(
            (file) => file.fileName !== data.fileName,
          ),
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
