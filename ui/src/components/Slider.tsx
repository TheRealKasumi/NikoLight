import * as SliderPrimitive from '@radix-ui/react-slider';
import classNames from 'classnames';
import {
  useController,
  type FieldValues,
  type UseControllerProps,
} from 'react-hook-form';
import { twMerge } from 'tailwind-merge';

type SliderProps<T extends FieldValues> = Omit<
  SliderPrimitive.SliderProps,
  'name' | 'onBlur' | 'onValueChange' | 'ref' | 'value'
> &
  UseControllerProps<T>;

export const Slider = <T extends FieldValues>({
  className,
  ...props
}: SliderProps<T>): JSX.Element => {
  const { field } = useController(props);

  return (
    <SliderPrimitive.Root
      max={100}
      min={0}
      step={1}
      className={twMerge(
        'relative flex h-5 w-64 touch-none items-center',
        className,
      )}
      {...props}
      name={field.name}
      onBlur={field.onBlur}
      onValueChange={([value]) => field.onChange(value)}
      ref={field.ref}
      value={field.value ? [field.value] : undefined}
    >
      <SliderPrimitive.Track className="relative h-2 w-full grow rounded-full bg-cloud dark:bg-gray">
        <SliderPrimitive.Range className="absolute h-full rounded-full bg-green" />
      </SliderPrimitive.Track>
      <SliderPrimitive.Thumb
        className={classNames(
          'block h-5 w-5 cursor-pointer rounded-full bg-zinc dark:bg-neutral',
          'focus:outline-none focus-visible:ring focus-visible:ring-green focus-visible:ring-opacity-75',
        )}
      />
    </SliderPrimitive.Root>
  );
};
