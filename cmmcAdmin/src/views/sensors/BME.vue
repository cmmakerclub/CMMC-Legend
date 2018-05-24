<template>
  <div class="content">
    <section class="section">
      <div class="columns">
        <div class="column">
          <div class="heading">
            <h1 class="title">BME Configuration</h1>
          </div>
          <div v-if="server_response" class="notification is-primary">
            {{ server_response }}
          </div>
          <label class="label">Sensor Type</label>
          <div class="control">
            <label class="radio">
              <input type="radio" name="type" v-model="bme_type" value="280">
              BME280
            </label>
            <label class="radio">
              <input type="radio" name="type" v-model="bme_type" value="680">
              BME680
            </label>
          </div>
          <label class="label">I2C Address</label>
          <p class="control has-icon">
            <input class="input" placeholder="Address" value="" disabled/>
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
      loadSensor () {
        const context = this
        context.$http.get('/bme.json')
          .then((response) => response.json())
          .then((json) => {
            context.bme_pin = json.bme_pin
            context.bme_type = json.bme_type
            context.bme_addr = json.bme_addr
            context.enable = parseInt(json.bme_enable)
          })
          .catch((err) => {
            console.log(err)
          })
      },
      onSubmit () {
        let context = this
        let formData = new window.FormData()
        formData.append('bme_type', context.bme_type)
        formData.append('bme_addr', '0x77')
        formData.append('bme_pin', `${context.bme_pin}`)
        formData.append('bme_enable', context.enable ? '1' : '0')
        context.$http.post('/api/sensors/bme', formData)
          .then((response) => response.json())
          .then((json) => {
            context.server_response = 'Saved'
            context.loadSensor()
          })
          .catch((err) => {
            console.log(err)
          })
      }
    },
    data () {
      return {
        loading: false,
        server_response: '',
        bme_type: '',
        bme_pin: '',
        bme_addr: '',
        enable: '0',
      }
    },

    mounted () {
      console.log('mounted')
      this.loadSensor()
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
