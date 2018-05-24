<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">DHT Calibration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Sensor Type</label>
          <div class="control">
            <label class="radio">
              <input type="radio" name="type" v-model="dht_type" value="11">
              DHT11
            </label>
            <label class="radio">
              <input type="radio" name="type" v-model="dht_type" value="22">
              DHT22
            </label>
          </div>
          <label class="label">Pin</label>
          <p class="control has-icon">
            <input class="input" placeholder="Pin Number" v-model="dht_pin"/>
            <i class="fa fa-thermometer"></i>
          </p>
          <!--<label class="label">Offset</label>-->
          <!--<p class="control has-icon">-->
          <!--<input class="input" placeholder=""/>-->
          <!--<i class="fa fa-thermometer"></i>-->
          <!--</p>-->
          <div class="control">
            <input type="checkbox" v-model="enable"> enable
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
  export default {
    components: {},

    props: {},

    methods: {
      loadDht () {
        const context = this
        context.$http.get('/dht.json')
          .then((response) => response.json())
          .then((json) => {
            context.dht_pin = json.dht_pin
            context.dht_type = json.dht_type
            context.enable = parseInt(json.dht_enable)
          })
          .catch((err) => {
            console.log(err)
          })
      },
      onSubmit () {
        let context = this
        let formData = new window.FormData()
        formData.append('dht_pin', context.dht_pin)
        formData.append('dht_type', context.dht_type)
        formData.append('dht_enable', context.enable ? '1' : '0')
        context.$http.post('/api/sensors/dht', formData)
          .then((response) => response.json())
          .then((json) => {
            context.server_response = 'Saved'
            context.loadDht()
          })
          .catch((err) => {
            console.log(err)
          })
      }
    },
    data () {
      return {
        loading: false,
        dht_type: '22',
        dht_pin: '12',
        server_response: '',
        enable: '0'
      }
    },

    mounted () {
      console.log('mounted')
      this.loadDht()
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
