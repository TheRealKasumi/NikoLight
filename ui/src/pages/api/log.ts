import { useMutation, useQuery, useQueryClient } from '@tanstack/react-query';
import ky from 'ky';

export const LOG_API_URL = '/api/log';

export type Log = {
  size: number;
};

type Response = {
  status: number;
  message: string;
};

type DataResponse = {
  log: Log;
} & Response;

export const useLog = ({
  startByte,
  byteCount,
}: {
  startByte: number;
  byteCount: number;
}) =>
  useQuery({
    queryKey: [LOG_API_URL, { start: startByte, count: byteCount }],
    queryFn: async () =>
      await ky
        .get(`${LOG_API_URL}?start=${startByte}&count=${byteCount}`)
        .text(),
  });

export const useLogSize = () =>
  useQuery<DataResponse, Error, Log['size']>({
    queryKey: [LOG_API_URL, 'size'],
    queryFn: async () => await ky.get(`${LOG_API_URL}/size`).json(),
    select: (data) => data.log.size,
  });

export const useClearLog = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error>({
    mutationFn: async () => await ky.delete(LOG_API_URL).json(),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: [LOG_API_URL] });
    },
  });
};
