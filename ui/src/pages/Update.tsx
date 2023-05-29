import { useForm } from 'react-hook-form';
import { Trans, useTranslation } from 'react-i18next';
import { Button, InputType, Loading, Notification, Toast } from '../components';
import { useUpdate } from './api';

type FormData = {
  files: File[];
};

export const Update = (): JSX.Element => {
  const { t } = useTranslation();
  const {
    handleSubmit,
    control,
    formState: { isSubmitting, isValid },
    reset,
  } = useForm<FormData>({
    mode: 'onChange',
  });
  const { mutateAsync, isSuccess, isError, isLoading, error } = useUpdate();

  const onSubmit = handleSubmit(async (data) => {
    await mutateAsync(
      { file: data.files[0] },
      {
        onSuccess: () => {
          reset();
        },
      },
    );
  });

  return (
    <>
      {isSuccess && <Toast title={t('update.updateSuccessful')} />}

      {isError && <Notification state="error" message={error.message} />}

      {isLoading && <Loading overlay />}

      <p className="mb-6">
        <Trans
          i18nKey="update.desc.1"
          components={{
            github: (
              <a
                href="https://github.com/TheRealKasumi/NikoLight"
                target="_blank"
                rel="noreferrer"
              >
                https://github.com/TheRealKasumi/NikoLight
              </a>
            ),
          }}
        />
      </p>
      <p className="mb-6">{t('update.desc.2')}</p>
      <p className="mb-6">{t('update.desc.3')}</p>

      <form onSubmit={onSubmit}>
        <InputType<FormData>
          accept=".tup"
          className="mb-6"
          control={control}
          name="files"
          required
          rules={{
            required: true,
          }}
        />

        <Button type="submit" disabled={isSubmitting || !isValid}>
          {t('update.submit')}
        </Button>
      </form>
    </>
  );
};
