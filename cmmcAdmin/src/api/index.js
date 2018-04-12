import Vue from 'vue'

export function getAPConfig (context) {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get('/api/wifi/ap')
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getConfig (context, type = 'mqtt') {
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.get(`/api/${type}/config`)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveAPConfig (context, ssid, pass) {
  var formData = new window.FormData()
  formData.append('ap_ssid', ssid)
  formData.append('ap_pwd', pass)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/wifi/ap', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function saveMqttConfig (context, options) {
  var formData = new window.FormData()
  formData.append('h',    options.host)
  formData.append('usr',  options.username)
  formData.append('pwd',  options.password)
  formData.append('cid',  options.clientId)
  formData.append('port', options.port)
  formData.append('lwt',  options.lwt)
  formData.append('pub_rate_s', options.pubRateS)
  let promise = new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/mqtt', formData)
      .then((response) => response.json())
      .then((json) => {
        resolve(json)
      })
      .catch((err) => {
        console.log(err)
      })
  })
  return promise
}

export function getAccessPoints (context) {
  // to be tested
  let promise = new Vue.Promise((resolve, reject) => {
    let list = []
    let fin = []
    let out = {}
    console.log('calling...')
    context.$http.get('/api/wifi/scan').then((response) => response.json())
      .then((aps) => {

        console.log(aps)

        list.push(aps)
        for (let _aps of list) {
          _aps.forEach((v, k) => {
            out[v.name] = k
          })
        }
        for (let it of Object.keys(out)) {
          fin.push({name: it})
        }
        resolve(fin)
      })
      .catch((error) => {
        console.log('eroro', error.status, error)
        reject(error)
      })
  })

  return promise
}

export function saveWiFiConfig (context, ssid, pass) {
  const formData = new window.FormData()
  formData.append('sta_ssid', ssid)
  formData.append('sta_password', pass)
  return new Vue.Promise((resolve, reject) => {
    context.$http.post('/api/wifi/sta', formData)
      .then((response) => response.json())
      .then((response) => {
        console.log('resp=', response)
        resolve(response)
      }).catch((error) => {
      reject(error)
    })
  })
}
