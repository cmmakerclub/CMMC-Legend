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

const router = new VueRouter({ routes })
const store = new Vuex.Store(vuexStore)

new Vue({
  store,
  router,
  render: h => h(App),
}).$mount('#app')
