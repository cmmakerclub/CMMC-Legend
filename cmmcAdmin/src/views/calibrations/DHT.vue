<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">DHT Calibration</h1>
          </div>
          <label class="label">Start Offset</label>
          <p class="control has-icon">
            <input class="input" placeholder="Offset Temperature">
            <i class="fa fa-thermometer"></i>
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
        .then((resp) => resp.json())
        .then((json) => {
          console.log(json)
        })
        .catch((err) => {
          console.log('error', err)
        })
      }
    },
    data () {
      return {
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
