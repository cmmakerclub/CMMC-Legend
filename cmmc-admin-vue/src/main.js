import Vue from 'vue'
import Vuex from 'vuex'
import App from './App.vue'
import VueRouter from 'vue-router'
import routes from './routes'
import vuexStore from './vuex-store'
import API from './api'
import 'tailwindcss/dist/tailwind.css'
import '@fortawesome/fontawesome-pro/css/all.css'

Vue.config.productionTip = false
Vue.use(Vuex)
Vue.use(VueRouter)
Vue.prototype.$api = API
Vue.prototype.$Microgear = window.Microgear
let microgear = window.Microgear.create({
  key: '301bT7j6D1lkIq9',
  secret: 'QWhZCzsre0ggXdtuH7Jy0xU8w',
})

console.log(`main.js`)
microgear.on('message', (topic, msg) => {
  console.log(`on_message arrived`, topic, msg)
})
microgear.on('connected', () => {
  //microgear.setAlias(ALIAS);
  console.log(`on_connected to netpie.`)
})
microgear.on('present', event => {
  console.log(`on_present`, event)
})
microgear.on('absent', event => {
  console.log(`on_absent`, event)
})

microgear.connect('vuejs')

Vue.prototype.$mg = microgear

const router = new VueRouter({ routes })
const store = new Vuex.Store(vuexStore)

new Vue({
  store,
  router,
  render: h => h(App),
}).$mount('#app')
