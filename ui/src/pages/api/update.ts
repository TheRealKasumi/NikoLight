import { useMutation } from '@tanstack/react-query';
import ky from 'ky';

export const UPDATE_API_URL = '/api/update';

export type Update = {
  file: File;
};

type Response = {
  status: number;
  message: string;
};

export const useUpdate = () =>
  useMutation<Response, Error, Update>({
    mutationFn: async (data) => {
      const formData = new FormData();
      formData.append('file', data.file);
      return await ky
        .post(UPDATE_API_URL, { body: formData, timeout: false })
        .json();
    },
  });
