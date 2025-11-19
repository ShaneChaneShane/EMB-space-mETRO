module.exports = {
  purge: [],
  darkMode: false, // or 'media' or 'class'
  content: [
    "./public/**/*.{js,html}"
  ],
  theme: {
    extend: {
      screens: {
        'xs': '25rem',     // 400px
        '2xl': '100rem',   // 1600px  
        '3xl': '120rem',   // 1920px
      },
    },
  },
  variants: {
    extend: {},
  },
  plugins: [],
  "editor.quickSuggestions": {
    "strings": "on"
  }
}