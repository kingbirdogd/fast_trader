import '/component/warrant/panel.css'

class Panel extends React.Component {
  constructor(props) {
    super(props)
    
    this.state = {}
    this.state.totalNo = 20
    this.state.curNo = 10
    this.state.fields = this.props.fields // 储存更新了的策略结构
    this.state.curModule = Object.keys(this.props.fields)[0]
    
    // 储存最初的策略结构
    this.state.fieldsOriginal = {}
    for(var key in fields)
      this.state.fieldsOriginal[key] = fields[key].map(a => a.name);
    
    this.setFields = this.setFields.bind(this)
    this.setCurModule = this.setCurModule.bind(this)
    this.setFieldsOriginal = this.setFieldsOriginal.bind(this)
    this.setCurNo = this.setCurNo.bind(this)
  }
  
  componentDidMount() {}
  
  shouldComponentUpdate(nextProps, nextState) {return true}
  
  componentDidUpdate() {}
  
  componentWillUnmount() {}
  
  componentDidCatch() {}
  
  setFields(fields) {
    this.setState({fields: fields})
  }
  
  setCurModule(curModule) {
    this.setState({curModule: curModule})
  }
  
  setFieldsOriginal(fieldsOriginal) {
    this.setState({fieldsOriginal: fieldsOriginal})
  }
  
  setCurNo(curNo) {
    this.setState({curNo: curNo})
  }
  
  render() {
    return(
      <React.Fragment>
        <nav>
          <div className="nav nav-tabs" role="tablist">
            <a className="nav-item nav-link active" data-toggle="tab" href="#nav-panel" role="tab" aria-controls="nav-panel" aria-selected="true">Panel</a>
            <a className="nav-item nav-link" data-toggle="tab" href="#nav-configure" role="tab" aria-controls="nav-configure" aria-selected="false">Configure</a>
          </div>
        </nav>
        
        <div className="tab-content">
          <div className="tab-pane fade show active" id="nav-panel" role="tabpanel" aria-labelledby="nav-panel-tab">
            <TradePanel 
              prefix="u001_"
              fields={this.state.fields}
              fieldsOriginal={this.state.fieldsOriginal}
              totalNo={this.state.totalNo}
              curNo={this.state.curNo}
              curModule={this.state.curModule}
              lang = {this.props.lang}
              setFields = {this.setFields}
              setFieldsOriginal = {this.setFieldsOriginal}
              setCurModule = {this.setCurModule}
              setCurNo = {this.setCurNo}
            />
          </div>
          
          <div className="tab-pane fade" id="nav-configure" role="tabpanel" aria-labelledby="nav-configure-tab">
            <Configure
              prefix="c001_"
              fields={this.state.fields}
              curModule={this.state.curModule}
              lang = {this.props.lang}
              setFields = {this.setFields}
              setCurModule = {this.setCurModule}
            />
          </div>
        </div>
      </React.Fragment>
    )
  }
}

var lang = 'sc' /* global.lang */
ReactDOM.render(<Panel fields={fields} lang={lang} />, document.getElementById('panel'))