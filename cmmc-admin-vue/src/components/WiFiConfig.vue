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

        <div class="pt-8">
            <form class="w-full max-w-100 mx-auto">
                <div class="flex justify-end">
                    <p class="text-2xl text-gray-500">AP Configuration</p>
                </div>
                <br>

                <div class="max-w-lg mx-auto">
                    <div class="flex flex-wrap">
                        <div class="md:w-1/3">
                            <label for="ap-name"
                                   class="block text-indigo-500 font-bold md:text-right mb-1 md:mb-0 pr-4 pt-2">AP
                                Name</label>
                        </div>
                        <div class="md:w-2/3">
                            <input class="appearance-none border-2 border-gray-200 rounded w-full py-2 px-4 text-gray-700 leading-tight focus:outline-none focus:bg-white focus:border-purple-500"
                                   id="ap-name" type="text" value="">
                        </div>
                    </div>
                </div>
                <div class="max-w-lg mx-auto mt-4">
                    <div class="flex flex-wrap">
                        <div class="md:w-1/3">
                            <label for="password"
                                   class="block text-indigo-500 font-bold md:text-right mb-1 md:mb-0 pr-4 pt-2">
                                Password</label>
                        </div>
                        <div class="md:w-2/3">
                            <input class="appearance-none border-2 border-gray-200 rounded w-full py-2 px-4 text-gray-700 leading-tight focus:outline-none focus:bg-white focus:border-purple-500"
                                   id="password" type="text" value="">
                        </div>
                    </div>
                </div>

                <div class="max-w-lg mx-auto mt-4">
                    <div class="flex flex-wrap">
                        <div class="md:w-1/3">
                        </div>
                        <div class="md:w-2/3">
                            <div class="flex">
                                <button class="mr-1 w-full shadow bg-green-500 hover:bg-green-600 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded"
                                        type="button">
                                    Save
                                </button>
                                <button class="ml-1 w-full shadow bg-red-500 hover:bg-red-600 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded"
                                        type="button">
                                    Cancel
                                </button>
                            </div>
                        </div>
                    </div>
                </div>

                <!--                <div class="max-w-sm mx-auto">-->
                <!--                    <div class="md:flex md:items-center mb-6">-->
                <!--                        <div class="md:w-1/3">-->
                <!--                            <label class="block text-green-500 font-bold md:text-right mb-1 md:mb-0 pr-4"-->
                <!--                                   for="inline-full-name">-->
                <!--                                AP Name-->
                <!--                            </label>-->
                <!--                        </div>-->
                <!--                        <div class="md:w-2/3">-->
                <!--                            <input class="appearance-none border-2 border-gray-200 rounded w-full py-2 px-4 text-gray-700 leading-tight focus:outline-none focus:bg-white focus:border-purple-500"-->
                <!--                                   id="inline-full-name" type="text" value="">-->
                <!--                        </div>-->
                <!--                    </div>-->
                <!--                    <div class="md:flex md:items-center mb-6">-->
                <!--                        <div class="md:w-1/3">-->
                <!--                            <label class="block text-green-500 font-bold md:text-right mb-1 md:mb-0 pr-4"-->
                <!--                                   for="inline-username">-->
                <!--                                Password-->
                <!--                            </label>-->
                <!--                        </div>-->
                <!--                        <div class="md:w-2/3">-->
                <!--                            <input class="appearance-none border-2 border-gray-200 rounded w-full py-2 px-4 text-gray-700 leading-tight focus:outline-none focus:bg-white focus:border-purple-500"-->
                <!--                                   id="inline-username" type="password" placeholder="">-->
                <!--                        </div>-->
                <!--                    </div>-->
                <!--                    <div class="md:flex md:items-center">-->
                <!--                        <div class="md:w-1/3"></div>-->
                <!--                        <div class="md:w-2/3">-->
                <!--                                            <button class="shadow bg-green-500 hover:bg-green-600 focus:shadow-outline focus:outline-none text-white font-bold py-2 px-4 rounded"-->
                <!--                                                    type="button">-->
                <!--                                                Save-->
                <!--                                            </button>-->
                <!--                        </div>-->
                <!--                    </div>-->
                <!--                </div>-->
            </form>
        </div>
    </div>
</template>

<script>
  export default {
    name: 'WiFiConfig',
  }
</script>
