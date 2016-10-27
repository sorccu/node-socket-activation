{
  'targets': [
    {
      'target_name': 'socket_activation',
      'include_dirs': [
        '<!(node -e \'require("nan")\')'
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'MACOSX_DEPLOYMENT_TARGET': '10.9'
          },
          'sources': [
            'src/launchd.cc'
          ]
        }]
      ]
    }
  ]
}
