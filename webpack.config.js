var path = require('path')

module.exports = {
    mode: 'development',
    resolve: {
        fallback: {
            'path': require.resolve('path-browserify'),
            'url': require.resolve('url/')
        }
    },
    module: {
        rules: [
            {
                test: /\.css$/,
                use: [{ loader: 'style-loader' }, { loader: 'css-loader' }],
            },
        ],
    },
    output: {
        path: path.resolve(__dirname, 'public'),
    },
}