import axios from 'axios'

let domain = 'localhost:8000'

let url = {
  wifi: {
    ap: '/api/wifi/ap',
  },
}

const api = {
  getAPConfig: function () {
    axios.get(domain + url.wifi.ap).then((response) => {
      console.log(response)
      return response
    }).catch((error) => console.log(error))
  },
}

export default api
