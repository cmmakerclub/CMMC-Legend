<template>

  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">

          <form action="#">
            <div class="heading">
              <h1 class="title">MQTT CONFIGURATION</h1>
            </div>
            <div v-if="server_response" class="notification is-primary">
              {{ server_response }}
            </div>
            <label class="label">Host</label>
            <p class="control has-icon">
              <input class="input" type="text" v-model="h" required>
              <i class="fa fa-connectdevelop"></i>
            </p>
            <label class="label">Port</label>
            <p class="control has-icon">
              <input class="input" type="number" v-model="port" required>
              <i class="fa fa-exchange"></i>
            </p>
            <label class="label">ClientID</label>
            <p class="control has-icon">
              <input class="input" type="text" v-model="cid" required>
              <i class="fa fa-vcard-o"></i>
            </p>
            <label class="label">Username</label>
            <p class="control has-icon">
              <input class="input" type="text" v-model="usr">
              <i class="fa fa-user"></i>
            </p>
            <label class="label">Password</label>
            <p class="control has-icon">
              <input class="input" type="password" v-model="pwd">
              <i class="fa fa-lock"></i>
            </p>
            <label class="label">PUBLISH_EVERY_S</label>
            <p class="control has-icon">
              <input class="input" type="number" v-model="pubRateS" required>
              <i class="fa fa-exchange"></i>
            </p>
            <div class="control">
              <input type="checkbox" v-model="lwt"> enable LWT
            </div>
            <div class="control">
              <button class="button is-primary" v-on:click="onSubmit">Submit</button>
              <button class="button is-link">Cancel</button>
            </div>
          </form>

        </div>
      </div>
    </section>
  </div>

</template>

<script>
  import { saveMqttConfig, getConfig } from '../../api'

  export default {
    components: {},
    methods: {
      onSubmit () {
        let context = this
        console.log('context', context)
        saveMqttConfig(context, {
          host: context.h,
          username: context.usr,
          password: context.pwd,
          clientId: context.cid,
          pubRateS: context.pubRateS,
          lwt: context.lwt,
          port: context.port,
        })
          .then((resp) => {
            this.server_response = resp
          })
          .catch((err) => {
            console.log('error', err)
          })
      },
      fetchConfig () {
        let ctx = this
        getConfig(ctx).then((configs) => {
          Object.entries(configs).forEach(([key, value]) => {
            ctx[key] = value
          })
        })
      }
    },
    data () {
      return {
        server_response: '',
        usr: '',
        pwd: '',
        lwt: 0,
        cid: `clientId-${Math.random().toString(15).substr(2, 10)}`,
        h: 'mqtt.cmmc.io',
        pubRateS: 10000,
        port: 1883,
      }
    },
    created () {
      console.log('on created.')
      let ctx = this
      ctx.loadConfig = () => {
        ctx.loading = true
        ctx.fetchConfig()
      }
    },
    mounted () {
      this.loadConfig()
    }
  }
</script>

<style scoped>
  .button {
    margin: 5px 0 0;
  }

  .control .button {
    margin: inherit;
  }
</style>
