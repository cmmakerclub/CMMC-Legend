<template>
    <div>
        <ul class="flex border-b">
            <li class="-mb-px mr-1">
                <span class="bg-white inline-block py-2 px-4 text-blue-700 font-semibold"
                      :class="{'border-l border-t border-r rounded-t' : isTabToggle}"
                      style="cursor: pointer"
                      @click="tabToggle">AP</span>
            </li>
            <li class="mr-1">
                <span class="bg-white inline-block py-2 px-4 text-blue-500 hover:text-blue-800 font-semibold"
                      :class="{'border-l border-t border-r rounded-t' : !isTabToggle}"
                      @click="tabToggle">STA</span>
            </li>
        </ul>

        <div v-if="isTabToggle">
            <AP/>
        </div>

        <div v-if="!isTabToggle">
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
        isTabToggle: true,
      }
    },
    methods: {
      tabToggle: function () {
        this.isTabToggle = !this.isTabToggle
      },
    },
    mounted () {
      this.$api.getAPConfig()
    },
  }
</script>
