<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">ESP Now Configuration</h1>
          </div>
          <label class="label">Mac Address</label>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <p class="control has-icon">
            <input class="input" type="text" v-model="ssid">
            <i class="fa fa-address-card-o"></i>
          </p>
          <label class="label">Mode</label>
          <div class="control">
            <label class="radio">
              <input type="radio" name="answer">
              Master
            </label>
            <label class="radio">
              <input type="radio" name="answer">
              Slave
            </label>
          </div>
          <p class="control">
            <button class="button is-primary" v-on:click="onSubmit">Submit</button>
            <button class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
  import { saveAPConfig, getAPConfig } from '../../api'

  export default {
    components: {},

    props: {},

    mounted () {
      getAPConfig(this).then((json) => {
        this.ssid = json.ap_ssid
        this.password = json.ap_pwd
      })
        .catch((err) => {
          console.log('error:', err)
        })
    },
    methods: {
      onSubmit () {
        let context = this
        saveAPConfig(context, context.ssid, context.password)
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
        server_response: null,
        loading: false,
        post: {},
        ssid: '',
        selected: '',
        password: ''
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
