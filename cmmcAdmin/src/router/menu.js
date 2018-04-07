// show: meta.label -> name
// name: component name
// meta.label: display label

// lazy loading Components
// https://github.com/vuejs/vue-router/blob/dev/examples/lazy-loading/app.js#L8
// const lazyLoading = (name, index = false) => () => System.import(`views/${name}${index ? '/index' : ''}.vue`)

export default [
  {
    name: 'WiFi Configuration',
    path: '/wifi',
    meta: {
      icon: 'fa-wifi',
      expanded: true
    },
    component: require('../views/charts'),

    children: [
      {
        name: 'AP',
        path: 'ap',
        component: require('../views/wifi/AP')
      },
      {
        name: 'STA',
        path: 'sta',
        component: require('../views/wifi/STA')
      }
    ]
  },
  {
    name: 'Configurations',
    path: '/config',
    meta: {
      icon: 'fa-life-saver',
      expanded: false
    },
    component: require('../views/charts'),
    children: [
      {
        name: 'NTP',
        path: 'general',
        component: require('../views/wifi/Form')
      },
      {
        name: 'MQTT',
        path: 'mqtt',
        component: require('../views/wifi/Form')
      }
    ]
  },
  {
    name: 'Calibrations',
    path: '/calibrate',
    meta: {
      icon: 'fa-sort',
      expanded: false
    },
    component: require('../views/charts'),
    children: [
      {
        name: 'DHT',
        path: 'dht',
        component: require('../views/calibrations/DHT')
      }
    ]
  },
  {
    name: 'OTA',
    path: '/ota',
    meta: {
      icon: 'fa-bank',
      expanded: false
    },
    component: require('../views/charts'),
    children: []
  },
  {
    name: 'Deep Sleep',
    path: '/deep',
    meta: {
      icon: 'fa-beer',
      expanded: false
    },
    component: require('../views/charts'),
    children: [ ]
  }
]
