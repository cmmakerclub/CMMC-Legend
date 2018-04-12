<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">STA Configuration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <p class="control" v-bind:class="{'is-loading': loading}">
              <span class="select">
                <select v-model="ssid">
                    <!--<option value="" disabled ssid hidden>Please Choose</option>-->
                    <option v-for="(val, idx) in access_points">
                      {{ val.name }}
                    </option>
                </select>
              </span>
            <a class="button" v-bind:class="{ 'is-loading': loading}" v-on:click="onRefresh">
                <span class="icon is-small">
                  <i class="fa fa-refresh"></i>
                </span>
              <span>Refresh</span>
            </a>
          </p>
          <label class="label">Manual SSID</label>
          <p class="control has-icon">
            <input v-model="manualSSID" class="input" type="text" placeholder="SSID">
            <i class="fa fa-wifi"></i>
          </p>
          <label class="label">Password</label>
          <p class="control has-icon">
            <input v-model="password" class="input" type="password" placeholder="Password">
            <i class="fa fa-lock"></i>
          </p>
          <p class="control">
            <button v-bind:class="{'is-loading': saving, 'is-disabled': loading||saving}" class="button is-primary"
                    v-on:click="onSubmit">Submit
            </button>
            <button class="button is-link">Cancel</button>
          </p>
        </div>
      </div>
    </section>
  </div>
</template>

<script>
  import { saveSTAConfig, getSTAConfig } from '../../api'

  export default {
    components: {},
    props: {},
    mounted () {
      getSTAConfig(this).then((json) => {
        this.ssid = json.sta_ssid
        this.password = json.sta_password
        this.manualSSID = json.sta_manual_ssid
      })
        .catch((err) => {
          console.log('error:', err)
        })
    },
    methods: {
      onRefresh () {
        console.log('on refresh')
        this.reload()
      },
      onSubmit () {
        let context = this
        this.saving = true
        saveSTAConfig(context, context.ssid, context.password, context.manualSSID)
          .then((resp) => {
            this.server_response = JSON.stringify(resp)
            this.saving = false
          })
          .catch((err) => {
            this.saving = false
            console.log(err)
          })
      },
      fetchAPs () {
        let ctx = this
        getAccessPoints(ctx).then((aps) => {
          aps.forEach((ap, k) => {
            this.map.set(ap.name, ap)
          })
          this.access_points = []
          for (let [key, value] of this.map) {
            console.log(key, value)
            this.access_points.push(value)
            this.loading = false
            this.ssid = this.access_points[0].name
          }
        })
      }
    },
    data () {
      return {
        ssid: null,
        password: null,
        manualSSID: null
      }
    },
    computed: {
      now: function () {
        return Date.now()
      }
    },
    created () {
      this.map = new Map()
      this.reload = () => {
        this.loading = true
        this.fetchAPs()
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
