import { PlayIcon, StopIcon, TrashIcon } from '@heroicons/react/24/outline';
import { useQueryErrorResetBoundary } from '@tanstack/react-query';
import { Suspense, useState } from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { useForm } from 'react-hook-form';
import { useTranslation } from 'react-i18next';
import {
  AnimationType,
  Button,
  Error,
  InputText,
  InputType,
  Loading,
  Notification,
  Toast,
} from '../components';
import { toMbyte } from '../libs';
import {
  useDeleteFseq,
  useFseq,
  useLed,
  useUpdateFseq,
  useUpdateLed,
  type Fseq,
} from './api';

type FormData = {
  files: File[];
  fileName: string;
};

const Form = (): JSX.Element => {
  const { t } = useTranslation();

  const [currrentFileId, setCurrentFileId] = useState<number | undefined>(
    undefined,
  );

  const {
    handleSubmit,
    control,
    formState: { isSubmitting },
    reset,
  } = useForm<FormData>({
    defaultValues: {
      fileName: '',
    },
    mode: 'onChange',
  });

  const { data: led } = useLed({
    onSuccess: (data) => {
      const zone = data[0];
      const animationSettings = zone.animationSettings;

      let fileId = 0;
      for (let i = 3; i >= 0; i--) {
        fileId = fileId * 256 + animationSettings[i + 20];
      }

      if (fileId > 0) {
        setCurrentFileId(fileId);
      }
    },
  });

  const {
    mutate: updateLed,
    isError: isUpdateError,
    error: updateError,
  } = useUpdateLed();

  const { data: fseq } = useFseq();

  const {
    mutateAsync: updateFseq,
    isLoading: isUploadLoading,
    isSuccess: isUploadSuccess,
    isError: isUploadError,
    error: uploadError,
  } = useUpdateFseq();

  const {
    mutate: deleteFseq,
    isError: isDeleteError,
    isSuccess: isDeleteSuccess,
    error: deleteError,
  } = useDeleteFseq();

  const onSubmit = handleSubmit(async (data) => {
    await updateFseq(
      {
        file: data.files[0],
        fileName: data.fileName,
      },
      {
        onSuccess: () => {
          reset();
        },
      },
    );
  });

  const onDelete = (file: Fseq) => deleteFseq({ fileName: file.fileName });

  const onPlay = (file: Fseq) => {
    let fileId = file.fileId;

    const zones = JSON.parse(JSON.stringify(led)) as NonNullable<typeof led>;
    const zone = zones[0];

    const prevType = zone.type;

    zone.type = AnimationType.FSEQ;

    // Write the fileId into the custom fields
    for (let i = 0; i < 4; i++) {
      const byte = fileId & 0xff;
      zone.animationSettings[i + 20] = byte;
      fileId = (fileId - byte) / 256;
    }

    // Set the last custom field to the previous calculated animation
    if (prevType !== AnimationType.FSEQ) {
      zone.animationSettings[24] = prevType;
    }

    zones[0] = zone;

    updateLed(zones, {
      onSuccess: () => {
        setCurrentFileId(file.fileId);
      },
    });
  };

  const onReset = () => {
    const zones = JSON.parse(JSON.stringify(led)) as NonNullable<typeof led>;
    const zone = zones[0];

    zone.type = zone.animationSettings[24];

    for (let i = 0; i < 4; i++) {
      zone.animationSettings[i + 20] = 0;
    }

    zones[0] = zone;

    updateLed(zones, {
      onSettled: () => {
        setCurrentFileId(undefined);
      },
    });
  };

  return (
    <>
      {isUploadSuccess && (
        <Toast title={t('customAnimations.uploadSuccessful')} />
      )}

      {isDeleteSuccess && (
        <Toast title={t('customAnimations.deleteSuccessful')} />
      )}

      {isUploadError && (
        <Notification state="error" message={uploadError.message} />
      )}

      {isDeleteError && (
        <Notification state="error" message={deleteError.message} />
      )}

      {isUpdateError && (
        <Notification state="error" message={updateError.message} />
      )}

      {isUploadLoading && <Loading overlay />}

      <form onSubmit={onSubmit}>
        {(fseq?.length ?? 0) > 0 && (
          <table className="mb-6 w-full">
            <thead className="text-left">
              <tr>
                <th className="w-2/4 py-2 pr-2 font-normal">
                  {t('customAnimations.name')}
                </th>
                <th className="p-2 font-normal">
                  {t('customAnimations.size')}
                </th>
                <th className="py-2 pl-2 font-normal" />
              </tr>
            </thead>
            <tbody>
              {fseq?.map((file) => (
                <tr key={file.fileId} className="border-t border-gray italic">
                  <td className="py-2 pr-2">{file.fileName}</td>
                  <td className="p-2">{toMbyte(file.fileSize)} MB</td>
                  <td className="py-2 pl-2 text-right">
                    {file.fileId === currrentFileId ? (
                      <>
                        <StopIcon
                          className="mr-2 inline-block h-5 w-5 cursor-pointer"
                          onClick={() => onReset()}
                        />
                        <TrashIcon className="inline-block h-5 w-5 cursor-not-allowed text-gray" />
                      </>
                    ) : (
                      <>
                        <PlayIcon
                          className="mr-2 inline-block h-5 w-5 cursor-pointer"
                          onClick={() => onPlay(file)}
                        />
                        <TrashIcon
                          className="inline-block h-5 w-5 cursor-pointer"
                          onClick={() => onDelete(file)}
                        />
                      </>
                    )}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        )}

        <label className="mb-6 flex flex-col">
          <span className="mb-2">{t('customAnimations.fileName')}</span>
          <InputText<FormData>
            control={control}
            maxLength={32}
            minLength={2}
            name="fileName"
            required
            rules={{
              required: true,
            }}
          />
        </label>

        <label className="mb-6 flex flex-col">
          <span className="mb-2">{t('customAnimations.file')}</span>
          <InputType<FormData>
            accept=".fseq"
            control={control}
            name="files"
            required
            rules={{
              required: true,
            }}
          />
        </label>

        <Button type="submit" disabled={isSubmitting}>
          {t('customAnimations.upload')}
        </Button>
      </form>
    </>
  );
};

export const CustomAnimations = (): JSX.Element => {
  const { reset } = useQueryErrorResetBoundary();

  return (
    <ErrorBoundary FallbackComponent={Error} onReset={reset}>
      <Suspense fallback={<Loading />}>
        <Form />
      </Suspense>
    </ErrorBoundary>
  );
};
