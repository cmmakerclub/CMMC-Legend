<template>
    <div>
        <ul class="flex border-b">
            <li class="-mb-px mr-1">
                <a class="bg-white inline-block border-l border-t border-r rounded-t py-2 px-4 text-blue-700 font-semibold"
                   href="#">AP</a>
            </li>
            <li class="mr-1">
                <a class="bg-white inline-block py-2 px-4 text-blue-500 hover:text-blue-800 font-semibold"
                   href="#">STA</a>
            </li>
        </ul>

        <AP/>

    </div>
</template>

<script>
  import AP from './AP'
  import STA from './STA'

  export default {
    name: 'Main',
    components: {
      AP,
      STA,
    },
  }
</script>
