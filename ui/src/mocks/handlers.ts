import { rest } from 'msw';
import type { Fseq, Led, System, Ui, Wifi } from '../pages/api';
import { throttledRes } from './throttledRes';

const mock: {
  system: {
    systemConfig: System;
  };
  led: {
    ledConfig: Led[];
  };
  wifi: {
    wifiConfig: Wifi;
  };
  ui: {
    uiConfig: Ui;
  };
  motion: {
    motionSensorCalibration: {
      accXRaw: number;
      accYRaw: number;
      accZRaw: number;
      gyroXRaw: number;
      gyroYRaw: number;
      gyroZRaw: number;
      accXG: number;
      accYG: number;
      accZG: number;
      gyroXDeg: number;
      gyroYDeg: number;
      gyroZDeg: number;
    }; // TODO: Update type
  };
  fseq: {
    fileList: Fseq[];
  };
  log: {
    text: string;
  };
} = {
  system: {
    systemConfig: {
      logLevel: 1,
      lightSensorMode: 1,
      lightSensorThreshold: 5,
      lightSensorMinAmbientBrightness: 5,
      lightSensorMaxAmbientBrightness: 255,
      lightSensorMinLedBrightness: 0,
      lightSensorMaxLedBrightness: 255,
      lightSensorDuration: 6,
      regulatorPowerLimit: 24,
      regulatorHighTemperature: 70,
      regulatorCutoffTemperature: 85,
      fanMinPwmValue: 75,
      fanMaxPwmValue: 255,
      fanMinTemperature: 60,
      fanMaxTemperature: 80,
      fanMode: 0,
    },
  },
  led: {
    ledConfig: [...new Array(8)].map<Led>(() => ({
      ledPin: 13,
      ledCount: 2,
      type: 0,
      speed: 50,
      offset: 10,
      brightness: 50,
      reverse: false,
      fadeSpeed: 30,
      ledVoltage: 5,
      animationSettings: [
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,
      ],
      channelCurrents: [16, 16, 16],
    })),
  },
  wifi: {
    wifiConfig: {
      accessPointSsid: 'TesLight',
      accessPointPassword: 'TesLightPW',
      accessPointChannel: 1,
      accessPointHidden: false,
      accessPointMaxConnections: 1,
    },
  },
  ui: {
    uiConfig: {
      expertMode: false,
      firmware: '1.0.0',
      language: 'en',
      theme: 'dark',
    },
  },
  motion: {
    motionSensorCalibration: {
      accXRaw: 0,
      accYRaw: 0,
      accZRaw: 0,
      gyroXRaw: 0,
      gyroYRaw: 0,
      gyroZRaw: 0,
      accXG: 0,
      accYG: 0,
      accZG: 0,
      gyroXDeg: 0,
      gyroYDeg: 0,
      gyroZDeg: 0,
    },
  },
  fseq: {
    fileList: [
      {
        fileName: 'test.fseq',
        fileSize: 33430436,
        fileId: 2839768,
      },
    ],
  },
  log: {
    text: '00:00:00:000 [INFO] (src/mock) Some fake log data.\n',
  },
};

export const handlers = [
  // ---------------
  // Connection Test
  // ---------------

  rest.get('/api/connection_test', (_req, res, ctx) =>
    res(ctx.status(200), ctx.json({ status: 200, message: 'ok' })),
  ),

  // --------------------
  // System Configuration
  // --------------------

  rest.get('/api/config/system', (_req, res, ctx) => {
    console.debug(`Get system configuration: ${mock.system}`);
    return res(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok', ...mock.system }),
    );
  }),

  rest.post('/api/config/system', async (req, res, ctx) => {
    mock.system = await req.json();
    console.debug(`Post new system configuration: ${mock.system}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // ----------------------
  // LED/Zone Configuration
  // ----------------------

  rest.get('/api/config/led', (_req, res, ctx) => {
    console.debug(`Get LED configuration: ${mock.led}`);
    return res(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok', ...mock.led }),
    );
  }),

  rest.post('/api/config/led', async (req, res, ctx) => {
    mock.led = await req.json();
    console.debug(`Post new LED configuration: ${mock.led}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // ------------------
  // WiFi Configuration
  // ------------------

  rest.get('/api/config/wifi', (_req, res, ctx) => {
    console.debug(`Get wifi configuration: ${mock.wifi}`);
    return res(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok', ...mock.wifi }),
    );
  }),

  rest.post('/api/config/wifi', async (req, res, ctx) => {
    mock.wifi = await req.json();
    console.debug(`Post new wifi configuration: ${mock.wifi}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // ------------------
  // UI Configuration
  // ------------------

  rest.get('/api/config/ui', (_req, res, ctx) => {
    console.debug(`Get UI configuration: ${mock.ui}`);
    return res(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok', ...mock.ui }),
    );
  }),

  rest.post('/api/config/ui', async (req, res, ctx) => {
    mock.ui = await req.json();
    console.debug(`Post new UI configuration: ${mock.ui}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // -----
  // Motion Sensor Calibration Configuration
  // -----

  rest.get('/api/config/motion', (_req, res, ctx) => {
    console.debug(`Get motion configuration: ${mock.motion}`);
    return res(
      ctx.status(200),
      ctx.json({
        status: 200,
        message: 'ok',
        ...mock.motion,
      }),
    );
  }),

  rest.post('/api/config/motion', async (req, res, ctx) => {
    mock.motion = await req.json();
    console.debug(`Post new motion configuration: ${mock.motion}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  rest.patch('/api/config/motion', async (_req, res, ctx) => {
    console.debug(`Patch motion configuration: ${mock.motion}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // ---------------
  // FSEQ Management
  // ---------------

  rest.get('/api/fseq', async (_req, res, ctx) => {
    console.debug(`Get motion configuration: ${mock.motion}`);
    return res(
      ctx.status(200),
      ctx.json({
        status: 200,
        message: 'ok',
        ...mock.fseq,
      }),
    );
  }),

  rest.post('/api/fseq', async (req, _res, ctx) => {
    // eslint-disable-next-line @typescript-eslint/no-non-null-assertion
    const fileName = req.url.searchParams.get('fileName')!;
    const arrayBuffer = await req.arrayBuffer();
    mock.fseq.fileList.push({
      fileId: Math.floor(Math.random() * 1e5),
      fileSize: arrayBuffer.byteLength,
      fileName,
    });
    console.debug(`Post new fseq: ${fileName}`);
    return throttledRes(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok' }),
    );
  }),

  rest.delete('/api/fseq', async (req, res, ctx) => {
    const fileName = req.url.searchParams.get('fileName');
    mock.fseq.fileList = mock.fseq.fileList.filter(
      (file) => file.fileName !== fileName,
    );
    console.debug(`Delete fseq: ${mock.fseq}`);
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // ------
  // Update
  // ------

  rest.post('/api/update', async (_req, _res, ctx) => {
    console.debug('Post update');
    return throttledRes(
      ctx.status(200),
      ctx.json({ status: 200, message: 'ok' }),
    );
  }),

  // ---
  // Log
  // ---

  rest.get('/api/log/size', (_req, res, ctx) => {
    const logSize = mock.log.text.length * 5000;
    console.debug(`Get log size: ${logSize}`);
    return res(
      ctx.status(200),
      ctx.json({
        status: 200,
        message: 'ok',
        log: { size: logSize },
      }),
    );
  }),

  rest.get('/api/log', (req, res, ctx) => {
    let expandedLogData = '';
    for (let i = 0; i < 5000; i++) {
      expandedLogData = expandedLogData.concat(mock.log.text);
    }

    const start = parseInt(req.url.searchParams.get('start') ?? '-1');
    const count = parseInt(req.url.searchParams.get('count') ?? '-1');

    if (start < 0 || count < 0 || start + count > expandedLogData.length) {
      console.error('Invalid request to read the log, returning 400.');
      return res(ctx.status(400));
    }

    let reply = '';
    for (let i = start; i < start + count; i++) {
      reply = reply.concat(expandedLogData.charAt(i));
    }

    console.debug(`Get log with a length of ${count} bytes.`);
    return res(ctx.status(200), ctx.text(reply));
  }),

  rest.delete('/api/log', (_req, res, ctx) => {
    console.debug('Delete log');
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  // -----
  // Reset
  // -----

  rest.post('/api/reset/soft', async (_req, res, ctx) => {
    console.debug('Post reset soft');
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),

  rest.post('/api/reset/hard', async (_req, res, ctx) => {
    console.debug('Post reset hard');
    return res(ctx.status(200), ctx.json({ status: 200, message: 'ok' }));
  }),
];
