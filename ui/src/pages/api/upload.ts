import { useMutation } from '@tanstack/react-query';
import ky from 'ky';

const API_URL = '/api/update';

export type Upload = {
  file: File;
};

type Response = {
  status: number;
  message: string;
};

export const useUpload = () =>
  useMutation<Response, Error, Upload>({
    mutationFn: async (data) => {
      const formData = new FormData();
      formData.append('file', data.file);
      return await ky.post(API_URL, { body: formData, timeout: false }).json();
    },
  });
