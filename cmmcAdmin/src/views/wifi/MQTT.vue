<template>

  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">MQTT CONFIGURATION</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Host</label>
          <p class="control has-icon">
            <input required class="input" type="text" placeholder="example mqtt.cmmc.io" v-model="host">
            <i class="fa fa-connectdevelop"></i>
          </p>
          <label class="label">Port</label>
          <p class="control has-icon">
            <input required class="input" type="number" v-model="port">
            <i class="fa fa-exchange"></i>
          </p>
          <label class="label">ClientID</label>
          <p class="control has-icon">
            <input required class="input" type="text" v-model="clientId">
            <i class="fa fa-vcard-o"></i>
          </p>
          <label class="label">Username</label>
          <p class="control has-icon">
            <input required class="input" type="text" placeholder="Username" v-model="username">
            <i class="fa fa-user"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input required class="input" type="password" placeholder="Password" v-model="password">
            <i class="fa fa-lock"></i>
          </p>
          <div class="control">
            <button class="button is-primary" v-on:click="onSubmit">Submit</button>
            <button class="button is-link">Cancel</button>
          </div>
        </div>
      </div>
    </section>
  </div>

</template>

<script>
  import {saveMqttConfig} from '../../api'
  export default {
    components: {},
    methods: {
      onSubmit () {
        let context = this
        saveMqttConfig(context, context.username, context.password, context.clientId, context.host, context.port)
          .then((resp) => {
            this.server_response = resp
          })
          .catch((err) => {
            console.log('error', err)
          })
      }
    },
    data () {
      return {
        server_response: '',
        username: '',
        password: '',
        clientId: `clientId-${Math.random().toString(15).substr(2, 10)}`,
        host: '',
        port: ''
      }
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
