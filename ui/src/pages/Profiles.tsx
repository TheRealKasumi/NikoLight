import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { Suspense } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import { useTranslation } from 'react-i18next';
import {
  Button,
  Error,
  InputText,
  Loading,
  Notification,
  Switch,
  Toast,
} from '../components';
import {
  Profile,
  useCloneProfile,
  useCreateProfile,
  useDeleteProfile,
  useProfile,
  useProfiles,
  useUpdateActiveProfile,
} from './api';

type FormData = {
  name: string;
  clone: boolean;
};

const Form = (): JSX.Element => {
  const { t } = useTranslation();
  const { data: profile } = useProfile();
  const { data: profiles } = useProfiles();

  const {
    mutateAsync: mutateAsyncCreateProfile,
    isSuccess: isCreateProfileSuccess,
    isError: isCreateProfileError,
    error: createProfileError,
  } = useCreateProfile();
  const {
    mutateAsync: mutateAsyncCloneProfile,
    isSuccess: isCloneProfileSuccess,
    isError: isCloneProfileError,
    error: cloneProfileError,
  } = useCloneProfile();
  const {
    mutate: mutateDeleteProfile,
    isSuccess: isDeleteProfileSuccess,
    isError: isDeleteProfileError,
    error: deleteProfileError,
  } = useDeleteProfile();
  const {
    mutate: mutateUpdateActiveProfile,
    isError: isUpdateActiveProfileError,
    error: updateActiveProfileError,
  } = useUpdateActiveProfile();

  const {
    handleSubmit,
    reset,
    control,
    formState: { isSubmitting, isValid },
  } = useForm<FormData>({
    defaultValues: {
      name: '',
      clone: false,
    },
    mode: 'onChange',
  });

  const onSubmit = handleSubmit(async (data) => {
    if (data.clone && profile?.name) {
      await mutateAsyncCloneProfile({ name: data.name, clone: profile.name });
    } else {
      await mutateAsyncCreateProfile({ name: data.name });
    }

    // Reset dirty fields
    reset({});
  });

  const onSelect = (name: Profile['name']) =>
    mutateUpdateActiveProfile({ name });

  const onDelete = (name: Profile['name']) => mutateDeleteProfile({ name });

  return (
    <>
      {isCreateProfileSuccess && (
        <Toast title={t('profiles.submitSuccessful')} />
      )}
      {isCloneProfileSuccess && (
        <Toast title={t('profiles.submitSuccessful')} />
      )}
      {isDeleteProfileSuccess && (
        <Toast title={t('profiles.deleteSuccessful')} />
      )}

      {isCreateProfileError && (
        <Notification state="error" message={createProfileError.message} />
      )}
      {isCloneProfileError && (
        <Notification state="error" message={cloneProfileError.message} />
      )}
      {isDeleteProfileError && (
        <Notification state="error" message={deleteProfileError.message} />
      )}
      {isUpdateActiveProfileError && (
        <Notification
          state="error"
          message={updateActiveProfileError.message}
        />
      )}

      <form className="mb-12" onSubmit={onSubmit}>
        <fieldset className="mb-6">
          <legend className="mb-6 text-lg font-medium">
            {t('profiles.new')}
          </legend>

          <label className="mb-6 flex flex-row">
            <span className="basis-1/2 self-center">{t('profiles.name')}</span>
            <div className="basis-1/2">
              <InputText<FormData>
                control={control}
                name="name"
                required
                rules={{
                  required: true,
                  validate: (_, formData) =>
                    !profiles?.names.includes(formData.name),
                }}
              />
            </div>
          </label>

          <label className="flex flex-row">
            <span className="basis-1/2 self-center">{t('profiles.clone')}</span>
            <div className="basis-1/2 text-right">
              <Switch<FormData> control={control} name="clone" />
            </div>
          </label>
        </fieldset>

        <Button type="submit" disabled={isSubmitting || !isValid}>
          {t('profiles.submit')}
        </Button>
      </form>

      <p className="mb-6 text-lg font-medium">{t('profiles.saved')}</p>
      {profiles?.names.map((profileName) => {
        const isActive = profile?.name === profileName;
        return (
          <div key={profileName} className="mb-6 flex flex-row">
            <span className="basis-1/2 self-center">{profileName}</span>
            <div className="flex basis-1/2 flex-row space-x-3">
              <Button
                disabled={isActive}
                type="button"
                variant="secondary"
                size="small"
                onClick={() => onDelete(profileName)}
              >
                {t('profiles.delete')}
              </Button>
              <Button
                disabled={isActive}
                type="button"
                size="small"
                onClick={() => onSelect(profileName)}
              >
                {t(isActive ? 'profiles.active' : 'profiles.select')}
              </Button>
            </div>
          </div>
        );
      })}
    </>
  );
};

export const Profiles = (): JSX.Element => {
  const { reset } = useQueryErrorResetBoundary();

  return (
    <ErrorBoundary FallbackComponent={Error} onReset={reset}>
      <Suspense fallback={<Loading />}>
        <Form />
      </Suspense>
    </ErrorBoundary>
  );
};
