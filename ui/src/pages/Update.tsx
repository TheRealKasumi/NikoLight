import { useForm } from 'react-hook-form';
import { Button, InputType, Loading, Notification, Toast } from '../components';
import { useUpload } from './api';

type FormData = {
  files: File[];
};

export const Update = (): JSX.Element => {
  const { handleSubmit, control, formState, reset } = useForm<FormData>({
    mode: 'onChange',
  });
  const { mutateAsync, isSuccess, isError, isLoading, error } = useUpload();

  const onSubmit = handleSubmit(
    async (data) =>
      await mutateAsync(
        { file: data.files[0] },
        {
          onSuccess: () => {
            reset();
          },
        },
      ),
  );

  return (
    <>
      {isSuccess && <Toast title="Update successfully!" />}

      {isError && <Notification message={error.message} />}

      {isLoading && <Loading overlay />}

      <p className="mb-6">
        Here a system update can be installed to the TesLight controller. This
        includes the firmware of the mcu as well as the user interface. To
        install a system update, you first need to download a so called
        &quot;TesLight Update Package&quot;. These files have the extension
        &quot;.tup&quot; and can be found in the release section on my GitHub
        page:{' '}
        <a
          href="https://github.com/TheRealKasumi/TesLight"
          target="_blank"
          rel="noreferrer"
        >
          https://github.com/TheRealKasumi/TesLight
        </a>
        .
      </p>
      <p className="mb-6">
        After the download, please use this page to upload the update package to
        the controller. Once the file is uploaded, the controller will restart
        and install the update. This can take up to 2 minutes but usually is
        faster. During this time, the controller will not be reachable via the
        WiFi hotspot. Please be patient. Afterwards the controller will restart
        and become reachable as usual.
      </p>
      <p className="mb-6">
        But here is also a word of warning. Uploading broken update packages
        could make the controller unreachable. Then a wired connection to
        reinstall the firmware is required. So it is recommended to only upload
        stable and tested update packages from the GitHub page.
      </p>

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

        <Button type="submit" disabled={formState.isSubmitting}>
          Update TesLight
        </Button>
      </form>
    </>
  );
};
