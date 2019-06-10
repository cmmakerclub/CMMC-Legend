import Vue from 'vue'
import App from './App.vue'
import VueRouter from 'vue-router'
import routes from './routes'
import 'tailwindcss/dist/tailwind.css'
import '@fortawesome/fontawesome-pro/css/all.css'

Vue.config.productionTip = false
Vue.use(VueRouter)

const router = new VueRouter({ routes })

new Vue({
  router,
  render: h => h(App),
}).$mount('#app')
