import {
  useMutation,
  useQuery,
  useQueryClient,
  UseQueryOptions,
} from '@tanstack/react-query';
import ky from 'ky';

export const PROFILE_API_URL = '/api/config/profile';

export type Profile = {
  name: string;
};

export type Profiles = {
  names: string[];
};

type CloneProfile = {
  name: string;
  clone: string;
};

type Response = {
  status: number;
  message: string;
};

type DataProfileResponse = {
  profile: Profile;
} & Response;

type DataProfilesResponse = {
  profile: Profiles;
} & Response;

type ProfileContext = {
  cache?: DataProfileResponse;
};

type ProfilesContext = {
  cache?: DataProfilesResponse;
};

export const useProfile = (
  options?: UseQueryOptions<DataProfileResponse, Error, Profile>,
) =>
  useQuery<DataProfileResponse, Error, Profile>({
    queryKey: [PROFILE_API_URL, 'active'],
    queryFn: async () => await ky.get(`${PROFILE_API_URL}/active`).json(),
    select: (data) => data.profile,
    ...options,
  });

export const useProfiles = (
  options?: UseQueryOptions<DataProfilesResponse, Error, Profiles>,
) =>
  useQuery<DataProfilesResponse, Error, Profiles>({
    queryKey: [PROFILE_API_URL],
    queryFn: async () => await ky.get(PROFILE_API_URL).json(),
    select: (data) => data.profile,
    ...options,
  });

export const useCreateProfile = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Profile, ProfilesContext>({
    mutationFn: async (data) =>
      await ky.post(PROFILE_API_URL, { json: { profile: data } }).json(),
    onMutate: async (profile) => {
      await queryClient.cancelQueries({ queryKey: [PROFILE_API_URL] });
      const cache = queryClient.getQueryData<DataProfilesResponse>([
        PROFILE_API_URL,
      ]);

      if (cache) {
        queryClient.setQueryData<DataProfilesResponse>([PROFILE_API_URL], {
          ...cache,
          profile: {
            names: [...cache.profile.names, profile.name],
          },
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataProfilesResponse>(
          [PROFILE_API_URL],
          context.cache,
        );
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [PROFILE_API_URL] });
    },
  });
};

export const useCloneProfile = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, CloneProfile, ProfilesContext>({
    mutationFn: async ({ name, clone }) =>
      await ky
        .put(PROFILE_API_URL, {
          json: { profile: { name } },
          searchParams: { name: clone },
        })
        .json(),
    onMutate: async (profile) => {
      await queryClient.cancelQueries({ queryKey: [PROFILE_API_URL] });
      const cache = queryClient.getQueryData<DataProfilesResponse>([
        PROFILE_API_URL,
      ]);

      if (cache) {
        queryClient.setQueryData<DataProfilesResponse>([PROFILE_API_URL], {
          ...cache,
          profile: {
            names: [...cache.profile.names, profile.name],
          },
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataProfilesResponse>(
          [PROFILE_API_URL],
          context.cache,
        );
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [PROFILE_API_URL] });
    },
  });
};

export const useUpdateActiveProfile = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Profile, ProfileContext>({
    mutationFn: async (data) =>
      await ky
        .patch(`${PROFILE_API_URL}/active`, { json: { profile: data } })
        .json(),
    onMutate: async (profile) => {
      await queryClient.cancelQueries({
        queryKey: [PROFILE_API_URL, 'active'],
      });
      const cache = queryClient.getQueryData<DataProfileResponse>([
        PROFILE_API_URL,
        'active',
      ]);

      if (cache) {
        queryClient.setQueryData<DataProfileResponse>(
          [PROFILE_API_URL, 'active'],
          {
            ...cache,
            profile,
          },
        );
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataProfileResponse>(
          [PROFILE_API_URL, 'active'],
          context.cache,
        );
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [PROFILE_API_URL, 'active'] });
    },
  });
};

export const useDeleteProfile = () => {
  const queryClient = useQueryClient();
  return useMutation<Response, Error, Profile, ProfilesContext>({
    mutationFn: async (data) =>
      await ky
        .delete(PROFILE_API_URL, {
          searchParams: { name: data.name },
        })
        .json(),
    onMutate: async (data) => {
      await queryClient.cancelQueries({ queryKey: [PROFILE_API_URL] });
      const cache = queryClient.getQueryData<DataProfilesResponse>([
        PROFILE_API_URL,
      ]);

      if (cache) {
        queryClient.setQueryData<DataProfilesResponse>([PROFILE_API_URL], {
          ...cache,
          profile: {
            ...cache.profile,
            names: cache.profile.names.filter((name) => name !== data.name),
          },
        });
      }
      return { cache };
    },
    onError: (_err, _variables, context) => {
      if (context?.cache) {
        queryClient.setQueryData<DataProfilesResponse>(
          [PROFILE_API_URL],
          context.cache,
        );
      }
    },
    onSettled: () => {
      queryClient.invalidateQueries({ queryKey: [PROFILE_API_URL] });
    },
  });
};
