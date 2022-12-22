import { compose, context, createResponseComposition } from 'msw';

export const throttledRes = createResponseComposition(undefined, [
  (res) => compose(context.delay(5000))(res),
]);
