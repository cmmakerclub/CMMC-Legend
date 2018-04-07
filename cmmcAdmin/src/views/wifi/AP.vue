<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">AP Configuration</h1>
          </div>
          <label class="label">AP</label>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <p class="control has-icon">
            <input class="input" placeholder="AP Name" v-model="ssid">
            <i class="fa fa-wifi"></i>
            {{ ssid }}
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input class="input" type="text" placeholder="Password" v-model="password">
            <i class="fa fa-lock"></i>
            {{ password }}
          </p>
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
  import {saveAPConfig, getAPConfig} from '../../api'
  export default {
    components: {},

    props: {},

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
    },

    mounted () {
      console.log('mounted')
      getAPConfig(this).then((json) => {
        this.ssid = json.ssid
        this.password = json.password
      })
      .catch((err) => {
        console.log('error:', err)
      })
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
