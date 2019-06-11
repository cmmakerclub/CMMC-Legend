<template>
    <div>
        <ul class="flex border-b">
            <li class="-mb-px mr-1">
                <span class="bg-white inline-block py-2 px-4 text-blue-700 font-semibold"
                      :class="{'border-l border-t border-r rounded-t' : isActiveAP}"
                      style="cursor: pointer"
                      @click="open_ap_tab">AP</span>
            </li>
            <li class="mr-1">
                <span class="bg-white inline-block py-2 px-4 text-blue-500 hover:text-blue-800 font-semibold"
                      :class="{'border-l border-t border-r rounded-t' : isActiveSTA}"
                      @click="open_sta_tab">STA</span>
            </li>
        </ul>

        <div v-if="isActiveAP">
            <AP/>
        </div>

        <div v-if="isActiveSTA">
            <STA/>
        </div>

    </div>
</template>

<script>
  import AP from './AP'
  import STA from './STA'

  export default {
    name: 'WiFiConfig',
    components: {
      AP,
      STA,
    },
    data: function () {
      return {
        isActiveAP: true,
        isActiveSTA: false,
      }
    },
    methods: {
      open_ap_tab: function () {
        this.isActiveAP = true
        this.isActiveSTA = false
      },
      open_sta_tab: function () {
        this.isActiveAP = false
        this.isActiveSTA = true
      },
    },
    mounted () {
      this.$api.getAPConfig()
    },
  }
</script>
