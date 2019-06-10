import Vue from 'vue'
import Vuex from 'vuex'
import App from './App.vue'
import VueRouter from 'vue-router'
import routes from './routes'
import store from './vuex-store'
import 'tailwindcss/dist/tailwind.css'
import '@fortawesome/fontawesome-pro/css/all.css'

Vue.config.productionTip = false
Vue.use(Vuex)
Vue.use(VueRouter)

const router = new VueRouter({ routes })

new Vue({
  store,
  router,
  render: h => h(App),
}).$mount('#app')
