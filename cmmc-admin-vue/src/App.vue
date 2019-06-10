<template>
    <div id="app">
        <div class="container max-w-4xl mx-auto">
            <Navbar/>
            <div class="min-h-screen border border-gray-300 pt-8 pl-8 pr-8">
                <WiFiConfig/>
            </div>
        </div>
    </div>
</template>

<script>
  import Navbar from './components/Navbar'
  import WiFiConfig from './components/wifi/Main'

  export default {
    name: 'app',
    components: {
      Navbar,
      WiFiConfig,
    },
  }
</script>

<style>

</style>
