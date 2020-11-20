class SignalTable extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {}
    this.handleReset = this.handleReset.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  getText(lang) {
    var text = {
      en: {id: 'ID', ucode: 'Underlying', ask: 'ask', wnt: 'wnt', reset: 'Reset'},
      sc: {id: 'ID', ucode: '相关资产', ask: '卖牌', wnt: '轮证', reset: '重設'},
      tc: {id: 'ID', ucode: '相關資產', ask: '賣牌', wnt: '輪証', reset: '重设'},
    }
    return text[lang]
  }
  
  handleReset() {
    var ucode = parseInt(event.target.name)
    var states = this.props.getStates()
    var userId = parseInt(states.userId)
    var algoName = (states.modules.call) ? states.modules.call : states.modules.put
    
    var command = {cmd: 'selectunderlying', algo_name: algoName, id: userId, ref: 'uid_'+userId.toString(), ucode: ucode, action: 'remove'}
    sendWebsocket(JSON.stringify(command))
    
    //
    var obj = $.extend(true, {}, states.signal)
    delete obj[ucode]
    this.props.setStates({signal: obj})
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var no = 0, rows1 = []
    for (const [ucode, d] of Object.entries(this.props.data)) {
      no++
      var uname = getUnderlyingName2(ucode)
      var ucode1 = formatCode(ucode, 4)
      //
      var rows2 = [], exist_products = []
      for (var d2 of d.detectedlist) {
        if (!exist_products.includes(d2[0])) {
          exist_products.push(d2[0])
          rows2.push(<td key={'signal_'+no+'_'+d2[0]+'_'+Math.floor((Math.random()*100000000)+1)}>{d2[0]}: {d2[1]}</td>)
        }
      }
      //
      var noShow=5
      if (rows2.length>noShow) rows2.length=noShow
      //
      while (rows2.length<noShow)
        rows2.push(<td key={'signal_'+no+'_'+rows2.length}>/</td>)
        
      //
      rows1.push(
        <tr key={'signal_'+no}>
          <td>{no}</td>
          <td>{ucode1} {uname}</td>
          <td>{d.ask}</td>
          {rows2}
          <td>
            <button
              name={ucode1}
              type="button"
              className="btn btn-sm btn-secondary"
              onClick={this.handleReset}>
                {text.reset}
            </button>
          </td>
        </tr>
      )
    }
    
    return(
      <div className='row'>
        <div className="col-12">
          <h6>Signal</h6>
          <table className="table table-sm table-striped table-light table-signal">
            <colgroup>
              <col span="1" width="50px" />
              <col span="1" width="200px" />
              <col span="1" width="100px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="200px" />
              <col span="1" width="50px" />
            </colgroup>
            <thead>
              <tr>
                <th>{text.id}</th>
                <th>{text.ucode}</th>
                <th>{text.ask}</th>
                <th>{text.wnt} 1</th>
                <th>{text.wnt} 2</th>
                <th>{text.wnt} 3</th>
                <th>{text.wnt} 4</th>
                <th>{text.wnt} 5</th>
                <th></th>
              </tr>
            </thead>
            <tbody>
              {rows1}
            </tbody>
          </table>
        </div>
      </div>
    )
  }
}